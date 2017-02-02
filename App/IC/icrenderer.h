/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#pragma once

#include "Core/renderer.h"
#include "CLW/clwscene.h"
#include "Scene/scene_tracker.h"
#include "radiance_cache.h"

#include "CLW.h"

namespace Baikal
{
    class ClwOutput;
    struct ClwScene;
    class SceneTracker;
    class RadianceCache;

    ///< Renderer implementation
    class IcRenderer : public Renderer
    {
    public:
        // Constructor
        IcRenderer(CLWContext context, int devidx, int num_bounces);
        // Destructor
        ~IcRenderer();
        // Renderer overrides
        // Create output
        Output* CreateOutput(std::uint32_t w, std::uint32_t h) const override;
        // Delete output
        void DeleteOutput(Output* output) const override;
        // Clear output
        void Clear(RadeonRays::float3 const& val, Output& output) const override;
        // Do necessary precalculation and initialization
        void Preprocess(Scene1 const& scene) override;
        // Render the scene into the output
        void Render(Scene1 const& scene) override;
        // Set output
        void SetOutput(Output* output) override;
        // Set number of light bounces
        void SetNumBounces(int num_bounces);
        // Interop function
        CLWKernel GetCopyKernel();
        // Add function
        CLWKernel GetAccumulateKernel();
        // Run render benchmark
        void RunBenchmark(Scene1 const& scene, std::uint32_t num_passes, BenchmarkStats& stats) override;


        // Debug code
        // Render the scene into the output
        void RenderCachedData(Scene1 const& scene);
        void VisualizeCache(ClwScene& scene);

    protected:
        // Resize output-dependent buffers
        void ResizeWorkingSet(Output const& output);
        // Generate rays
        void GeneratePrimaryRays(ClwScene const& scene);
        // Shade first hit
        void ShadeSurface(ClwScene const& scene, int pass);
        // Evaluate volume
        void EvaluateVolume(ClwScene const& scene, int pass);
        // Handle missing rays
        void ShadeMiss(ClwScene const& scene, int pass);
        // Gather light samples and account for visibility
        void GatherLightSamples(ClwScene const& scene, int pass);
        // Restore pixel indices after compaction
        void RestorePixelIndices(int pass);
        // Convert intersection info to compaction predicate
        void FilterPathStream(int pass);
        // Integrate volume
        void ShadeVolume(ClwScene const& scene, int pass);
        // Shade background
        void ShadeBackground(ClwScene const& scene, int pass);
        // 
        void GenerateProbeRequests(Scene1 const& scene, 
            CLWBuffer<RadianceCache::RadianceProbeDesc>& desc, 
            CLWBuffer<RadianceCache::RadianceProbeData>& probes,
            std::size_t& num_probes);
        //
        void UpdateRadianceCache(ClwScene const& scene, int pass, std::size_t maxrays);

    public:
        // CL context
        CLWContext m_context;
        // Output object
        ClwOutput* m_output;
        // Scene tracker
        SceneTracker m_scene_tracker;

        // GPU data
        struct PathState;
        struct RenderData;

        std::unique_ptr<RenderData> m_render_data;
        std::unique_ptr<RadianceCache> m_radiance_cache;

        // Intersector data
        std::vector<RadeonRays::Shape*> m_shapes;

        // Vidmem usage
        // Working set
        size_t m_vidmemws;

    private:
        std::uint32_t m_num_bounces;
        mutable std::uint32_t m_framecnt;
    };
}