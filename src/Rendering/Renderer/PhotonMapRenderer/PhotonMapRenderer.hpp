#pragma once

#include "../IRenderer.hpp"
#include "Builders/PhotonMapBuilder/IPhotonMapBuilder.hpp"
#include "Core/Color/Color.hpp"
#include "Core/Math/Interval.hpp"
#include "Geometry/Background/IBackground.hpp"
#include "Geometry/Hittable/HitData.hpp"
#include "Geometry/Light/ILight.hpp"
#include "Rendering/Framebuffer/Framebuffer.hpp"
#include "Rendering/LightSampler/ILightSampler.hpp"
#include "Rendering/PhotonMap/PhotonMap.hpp"
#include "Rendering/ProgressIndicator/IProgressIndicator.hpp"
#include "Rendering/Renderer/PhotonMapRenderer/PhotonMapRendererParameters.hpp"
#include "Rendering/Renderer/RendererStatistics.hpp"
#include "Utils/Logger/ILogger.hpp"

#include <memory>

namespace RTC {
class PhotonMapRenderer : public IRenderer {
private:
    std::shared_ptr<ILogger> logger_;
    std::unique_ptr<ILightSampler> lightSampler_;
    std::unique_ptr<IProgressIndicator> progressIndicator_;
    std::unique_ptr<IPhotonMapBuilder> photonMapBuilder_;
    PhotonMapRendererParameters parameters_;

    struct PhotonMaps {
        PhotonMap globalMap;
        PhotonMap causticMap;
    };

    [[nodiscard]] LinearColor getEmission(
        const HitData& hitData,
        const Vector3f& outDirection,
        uint32_t recursionDepth
    ) const;

    [[nodiscard]] LinearColor getIndirectLight(
        const HitData& hitData,
        const Vector3f& outDirection,
        const Scene& scene,
        const PhotonMaps& photonMaps,
        RendererStatistics& statistics,
        uint32_t recursionDepth
    ) const;

    void tracePhoton(
        Photon& photon,
        bool specularBounce,
        std::vector<Photon>& globalPhotons,
        std::vector<Photon>& causticPhotons,
        const Scene& scene,
        RendererStatistics& statistics,
        uint32_t recursionDepth = 0
    );

    [[nodiscard]] LinearColor traceRay(
        const Ray& ray,
        const Scene& scene,
        const PhotonMaps& photonMaps,
        RendererStatistics& statistics,
        uint32_t recursionDepth = 0
    ) const;

    void scatterPhotons(
        const Scene& scene,
        RendererStatistics& statistics,
        std::vector<Photon>& globalPhotons,
        std::vector<Photon>& causticPhotons
    );

    RendererStatistics renderSection(
        const Camera& camera,
        const Scene& scene,
        const PhotonMaps& photonMaps,
        const Interval<float>& renderInterval,
        const Interval<uint32_t>& xIndices,
        const Interval<uint32_t>& yIndices,
        Framebuffer& framebuffer
    ) const;

    std::vector<RendererStatistics> renderAll(
        const Camera& camera,
        const Scene& scene,
        const PhotonMaps& photonMaps,
        Framebuffer& framebuffer
    ) const;

public:
    PhotonMapRenderer(
        std::shared_ptr<ILogger> logger,
        std::unique_ptr<ILightSampler> lightSampler,
        std::unique_ptr<IProgressIndicator> progressIndicator,
        std::unique_ptr<IPhotonMapBuilder> photonMapBuilder,
        PhotonMapRendererParameters parameters
    );

    RendererStatistics render(
        const Camera& camera,
        Scene& scene,
        Framebuffer& framebuffer
    ) noexcept override;
};
}