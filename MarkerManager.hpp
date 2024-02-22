#pragma once

#include "result.hpp"
#include "Size.hpp"
#include "EigenUtils.hpp"
#include "vtkUtils.hpp"

class MarkerManager
{
    using CameraPointer = vtkSmartPointer<vtkCamera>;
    using FollowerPointer = vtkSmartPointer<vtkFollower>;
    using RendererPointer = vtkSmartPointer<vtkRenderer>;
    using TexturePointer = vtkSmartPointer<vtkTexture>;

    // ---- type traits for CreateMany with multiple arguments ----
    template<typename T>
    using is_eigen_vector = std::is_same<T, Eigen::Vector3d>;

    template<typename T>
    using is_std_double_array_ptr = std::is_same<T, double*>;

    template<typename ...Args>
    using are_eigen_vectors = std::conjunction<is_eigen_vector<Args>...>;

    template<typename ...Args>
    using are_std_double_array_ptrs = std::conjunction<is_std_double_array_ptr<Args>...>;

    template<typename T>
    using is_follower_pointer = std::is_same <T, FollowerPointer>;

    template<typename ...Ptrs>
    using are_follower_pointers = std::conjunction<is_follower_pointer<Ptrs>...>;

    template<typename T>
    using is_std_size_t = std::is_same<T, std::size_t>;

    template<typename ...Indices>
    using is_std_size_t_indices = std::conjunction<is_std_size_t<Indices>...>;
    // ---------
    
public:
    enum class MarkerType {
        Star, // Define purpose of this marker and rename it
        Circle, // Define purpose of this marker and rename it
        Path,
        Point, // Define purpose of this marker and rename it
        Normal,
        AntiNormal,
    };

    MarkerManager(const CameraPointer& camera, vtkRenderWindow* renderer)
        : _camera(camera), _renderWindow(renderer)
    {
        if(!_camera || !_camera.GetPointer())
            throw std::invalid_argument("Camera bound with MarkerManager can not be a null pointer");

        if(!_camera->GetParallelProjection())
            throw std::invalid_argument("MarkerManager implements managing logic only for cameras with parallel projection");

        _markers = new std::vector<FollowerPointer>();

        _camera->AddObserver(vtkCommand::ModifiedEvent, this, &MarkerManager::AdjustMarkersScale);
        _renderWindow->AddObserver(vtkCommand::ModifiedEvent, this, &MarkerManager::AdjustMarkersScale);
    }

    Result<FollowerPointer> CreateMarkerOfType(MarkerType type, const Eigen::Vector3d& position) noexcept
    {
        return CreateMarkerOfType(type, EigenVector3dToCArray(position));
    }

    Result<FollowerPointer> CreateMarkerOfType(MarkerType type, const double* position) noexcept
    {
        const auto texture = LoadMarkerTextureOfType(type);

        if(!texture.isSuccess())
            return { texture.status(), texture.message() };

        auto marker = CreateMarker(texture.value(), position);

        if(!marker.isSuccess())
            return { marker.status(), marker.message() };

        return marker;
    }

    Result<std::vector<FollowerPointer>> CreateMany(MarkerType type, 
        const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>& points) noexcept
    {
        std::vector<double*> tempPoints;
        tempPoints.reserve(points.size()); 

        for(const auto& pt : points) 
            tempPoints.push_back(EigenVector3dToCArray(pt));

        return CreateMany(type, tempPoints);
    }

    Result<std::vector<FollowerPointer>> CreateMany(MarkerType type, const std::vector<double*>& points) noexcept
    {
        std::vector<FollowerPointer> result {};
        result.reserve(points.size());

        for (const auto pt: points)
        {
            const auto marker = CreateMarkerOfType(type, pt);
            if(!marker.isSuccess())
                return { Status::GenericFailure, "Unable to create marker" };

            result.push_back(marker.value());
        }

        return result;
    }

    template<typename ...Points,
        std::enable_if_t<are_eigen_vectors<Points...>::value, bool> = true>
    Result<std::vector<FollowerPointer>> CreateMany(MarkerType type, const Points&... points) noexcept
    {
        std::vector<FollowerPointer> result;

        // Call CreateMarkerOfType for each one in points array and push result value to result vector
        (result.push_back(CreateMarkerOfType(type, points).value()), ...);

        return result;
    }

    template<typename ...Points,
        std::enable_if_t<are_std_double_array_ptrs<Points...>::value, bool> = true>
    Result<std::vector<FollowerPointer>> CreateMany(MarkerType type, Points... points) noexcept
    {
        std::vector<FollowerPointer> result;

        // Call CreateMarkerOfType for each one in points array and push result value to result vector
        (result.push_back(CreateMarkerOfType(type, points).value()), ...);

        return result;
    }

    Result<FollowerPointer> CreateWithUserTexture(const std::string& path, 
                                              const Eigen::Vector3d& position)
    {
        return CreateWithUserTexture(path, EigenVector3dToCArray(position));
    }

    Result<FollowerPointer> CreateWithUserTexture(const std::string& path,
                                              const double* position)
    {
        const auto texture = LoadVtkTexture(path);

        if(!texture.isSuccess())
            return { texture.status(), texture.message() };

        return CreateMarker(texture.value(), position);
    }

    /**
     * \brief Creates a variable count of markers with given texture at the given locations
     * \param texPath path to texture to load
     * \param points \c std::vector<Eigen::Vector3d> with allocator \c Eigen::aligned_allocator<Eigen::Vector3d> contains positions for markers
     * \return \c Result with \c std::vector<vtkSmartPointer<vtkFollower>> of created markers
     */
    Result<std::vector<FollowerPointer>> CreateManyWithUserTexture(const std::string& texPath,
            const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d>>& points)
    {
        std::vector<double*> tempPoints;
        tempPoints.reserve(points.size());

        for(const auto& pt : points)
            tempPoints.push_back(EigenVector3dToCArray(pt));

        return CreateManyWithUserTexture(texPath, tempPoints);
    }

    /**
     * \brief Creates a variable count of markers with given texture at the given locations
     * \param texPath path to texture to load
     * \param points \c std::vector<double*> with positions for markers
     * \return \c Result with \c std::vector<vtkSmartPointer<vtkFollower>> created markers
     */
    Result<std::vector<FollowerPointer>> CreateManyWithUserTexture(const std::string& texPath,
                                                                   const std::vector<double*>& points)
    {
        const auto texture = LoadVtkTexture(texPath);

        if(!texture.isSuccess())
            return { texture.status(), texture.message() };

        std::vector<FollowerPointer> result {};
        result.reserve(points.size());

        for(const auto pt : points) 
        {
            const auto marker = CreateMarker(texture.value(), pt);
            if(!marker.isSuccess())
                return { Status::GenericFailure, "Unable to create marker", result };

            result.push_back(marker.value());
        }

        return result;
    }

    //The single reason why i write this - i don't wanna copy Eigen::Vector3d for each method invokation
    //and also i dont want to duplicate full method only for pass vectors as const reference

    /**
     * \brief Creates a variable count of markers with given texture at the given locations
     * \tparam String Any string type that can be natively converted to \c std::string
     * \tparam Points Variable count of \c Eigen::Vector3d vectors defines positions for markers
     * \param texPath path to texture to load
     * \param points locations for markers
     * \return \c Result with \c std::vector<vtkSmartPointer<vtkFollower>> created markers
     */
    template<typename String, typename ...Points,
             std::enable_if_t<are_eigen_vectors<Points...>::value && std::is_convertible<String, std::string>::value, bool> = true>
    Result<std::vector<FollowerPointer>> CreateManyWithUserTexture(String&& texPath, const Points&... points)
    {
        return CreateManyWithUserTextureHelper(texPath, points...);
    }

    /**
     * \brief Creates a variable count of markers with given texture at the given locations
     * \tparam String Any string type that can be natively converted to \c std::string
     * \tparam Points Variable count of \c double* arrays defines positions for markers
     * \param texPath path to texture to load
     * \param points locations for markers
     * \return \c Result with \c std::vector<vtkSmartPointer<vtkFollower>> created markers
     */
    template<typename String, typename ...Points,
        std::enable_if_t<are_std_double_array_ptrs<Points...>::value && std::is_convertible<String, std::string>::value, bool> = true>
    Result<std::vector<FollowerPointer>> CreateManyWithUserTexture(String&& texPath, Points... points)
    {
        return CreateManyWithUserTextureHelper(texPath, points...);
    }

    void SetNewCamera(const CameraPointer& camera) noexcept
    {
        if(!camera || !camera.GetPointer())
            return;

        _camera = camera;

        for (const auto& marker: *_markers)
        {
            marker->SetCamera(_camera);
        }
    }

    const std::vector<FollowerPointer>* GetMarkers() const
    {
        return _markers;
    }

    void DeleteMarker(const FollowerPointer& marker)
    {
        const auto it = std::find(_markers->begin(), _markers->end(), marker);

        if(it != _markers->end())
            _markers->erase(it);
    }

    template<typename ...Markers,
        std::enable_if_t<are_follower_pointers<Markers...>::value, bool> = true>
    void DeleteMarkers(const Markers&... markers)
    {
        DeleteMarker(markers...);
    }

    void DeleteMarker(std::size_t idx)
    {
        if(idx < 0 || idx >= _markers->size())
            return;
        _markers->erase(_markers->begin() + idx);
    }

    template<typename ...I,
        std::enable_if_t<is_std_size_t_indices<I...>::value, bool> = true>
    void DeleteMarkers(I... indices)
    {
        DeleteMarker(indices...);
    }

    void CleanMarkers(const RendererPointer& owner)
    {
        for(const auto& marker: *_markers)
        {
            owner->RemoveActor(marker);
            marker->Delete();
        }

        _markers->clear();
    }

    void Update()
    {
        AdjustMarkersScale(nullptr, 0, nullptr); // Anyway we don't use this parameters
    }

private:
    // ------ constants -------
    const double _epsilon = 1e-8;
    const double _magicSizingFactor = 4;
    const double _markerPixelSize = 9;
    // -------------

    CameraPointer _camera;
    vtkRenderWindow* _renderWindow;
    std::vector<FollowerPointer>* _markers;

    double _lastParallelScale = -1;
    Size _lastWndSize {};

    void AdjustMarkersScale(vtkObject*, ulong, void*)
    {
        const double currentParallelScale = _camera->GetParallelScale();
        const int* windowSize = _renderWindow->GetSize();

        if(std::fabs(currentParallelScale - _lastParallelScale) < _epsilon &&
            _lastWndSize == Size(windowSize[0], windowSize[1])) // Do nothing if scale not changed
        {
            return;
        }

        const double aspectRatio = static_cast<double>(windowSize[0]) / windowSize[1];
        const double sizeInWorldCoords = (_markerPixelSize / windowSize[0]) * _camera->GetParallelScale() * aspectRatio;
        const double markerScale = sizeInWorldCoords * _magicSizingFactor;

        ApplyScale(markerScale);

        _lastParallelScale = currentParallelScale;

        _lastWndSize.Height() = windowSize[0];
        _lastWndSize.Width() = windowSize[1];
    }

    void ApplyScale(double scale) const
    {
        for (const auto& marker: *_markers)
        {
            marker->SetScale(scale);
        }
    }

    Result<FollowerPointer> CreateMarker(const TexturePointer& texture, const Eigen::Vector3d& position) const
    {
        return CreateMarker(texture, EigenVector3dToCArray(position));
    }

    Result<FollowerPointer> CreateMarker(const TexturePointer& texture, const double* position) const
    {
        texture->InterpolateOn();

        const auto planeSource = vtkSmartPointer<vtkPlaneSource>::New();
        planeSource->Update();

        const auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(planeSource->GetOutputPort());

        auto follower = vtkSmartPointer<vtkFollower>::New();
        follower->SetMapper(mapper);
        follower->SetTexture(texture);
        follower->SetCamera(_camera);

        follower->SetPosition(position[0], position[1], position[2]);

        _markers->push_back(follower);

        return follower;
    }

    template<typename String, typename Tuple,
        std::enable_if_t<std::is_convertible_v<String, std::string>, bool> = true,
        std::size_t ...I>
    Result<std::vector<FollowerPointer>> CreateManyWithUserTextureImpl(String&& texPath, Tuple&& points, std::index_sequence<I...>) noexcept
    {
        const std::string path = std::forward<String>(texPath);

        const auto texture = LoadVtkTexture(path);

        if(!texture.isSuccess())
            return { texture.status(), texture.message() };

        std::vector<FollowerPointer> result;

        // Call CreateMarker for each one in points array and push result value to result vector
        (result.push_back(CreateMarker(texture.value(), std::get<I>(points)).value()), ...);

        return result;
    }

    template<typename String, typename ...Points>
    Result<std::vector<FollowerPointer>> CreateManyWithUserTextureHelper(String&& texPath, Points&&... points)
    {
        return CreateManyWithUserTextureImpl(texPath, std::forward_as_tuple(points...),
            std::index_sequence_for<Points...>{});
    }

    static Result<vtkSmartPointer<vtkTexture>> LoadMarkerTextureOfType(MarkerType type)
    {
        //TODO: Replace string literals with external configuration
        switch(type)
        {
            case MarkerType::Circle:
                return LoadVtkTexture("node.png");
            case MarkerType::Path:
                return LoadVtkTexture("pathNode.png");
            case MarkerType::Star:
                return LoadVtkTexture("Star.png");
            case MarkerType::Normal:
                return LoadVtkTexture("Normal.png");
            case MarkerType::AntiNormal:
                return LoadVtkTexture("AntiNormal.png");
            default:
                return { Status::BadArgument, "Unsupported node type" };
        }
    }
};

using StdMarkers = MarkerManager::MarkerType;