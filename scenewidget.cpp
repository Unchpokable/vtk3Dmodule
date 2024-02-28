#include "pch.h"

#include "scenewidget.h"
#include "vtkUtils.hpp"
#include "MarkerManager.hpp"
#include "ProbeToolAssembly.hpp"

SceneWidget::SceneWidget(QWidget *parent, Qt::WindowFlags flags) : QVTKOpenGLNativeWidget(parent) {
    _renderer = vtkSmartPointer<vtkRenderer>::New();
    _renderer->GetActiveCamera()->ParallelProjectionOn();
    _renderer->GradientBackgroundOn();
    _renderer->SetBackground(0.3, 0.6, 0.6);
    _renderer->SetBackground2(.7, .7, .7);

    _renderer->AddObserver(vtkCommand::StartEvent, this, &SceneWidget::prepareToRender);

    vtkRenderWindow *window = renderWindow();
    window->AddRenderer(_renderer);
    window->SetPointSmoothing(true);
    window->SetLineSmoothing(true);
    window->SetPolygonSmoothing(true);

    _markerManager = new MarkerManager(_renderer->GetActiveCamera(), renderWindow());

    _renderTimer = new QTimer(this);
    _renderTimer->setInterval(0);
    _renderTimer->setSingleShot(true);

    connect(_renderTimer, &QTimer::timeout, this, &SceneWidget::renderScene);

    // -------------------------

    //Маркеры
    //const auto nodePosition = Eigen::Vector3d(10, 25, 14);

    //auto line = CreateLine({ 0, 0, 0 }, nodePosition, { 0, 1, 0 }); // Line from (0,0,0) to (10, 25, 14) with green color
    //auto line2 = CreateLine(nodePosition, { 43, 11, 35 }, { 0, 1, 0 });
    //auto line3 = CreateLine(nodePosition, { -20, -10, -8 }, { 0, 1, 0});
    //auto sphere = CreateSphere(10, { 0, 0, 0 }, { 1, 0, 0 });
    //auto sphere2 = CreateSphere(5, { 43, 11, 35 }, { 1, .3, .7 });
    //auto sphere3 = CreateSphere(8, { -20, -10, -8 }, { 0, 0, 1 });

    //_demoSpheres.push_back(sphere);
    //_demoSpheres.push_back(sphere2);
    //_demoSpheres.push_back(sphere3);

    //AddActorsToRenderer(_renderer, line, line2, line3, sphere, sphere2, sphere3);
    //const auto node = _markerManager->CreateMany(StdMarkers::Path, nodePosition).value().at(0);
    //const auto node2 = _markerManager->CreateMarkerOfType(StdMarkers::Circle, { 15, 35, 24 });
    //const auto node3 = _markerManager->CreateMarkerOfType(StdMarkers::Star, { 25, 45, 34 });

    //const auto nodePos2 = Eigen::Vector3d(45, 65, 54);
    //const auto nodePos3 = Eigen::Vector3d(55, 75, 64);

    //const auto antiNormalMarkers = _markerManager->CreateManyWithUserTexture("AntiNormal.png", nodePos2, nodePos3).value();

    //AddActorsToRenderer(_renderer, 
    //    node, 
    //    node2.value(), 
    //    node3.value(),
    //    _markerManager->CreateMarkerOfType(StdMarkers::Normal, { 35, 55, 44 }).value(),
    //    antiNormalMarkers.at(0),
    //    antiNormalMarkers.at(1)
    //    
    //);

    /*const auto cone = CreateTruncatedCone(10, 25, 14, { 60, 60, 60 }, { .5, .5, .5 }, DetailsLevel::High);

    const auto realCone = CreateCone(24, 50, { 50, 50, 50 }, { .5, .5, .5 });

    const auto cylinder = CreateCylinder(50, 25, { -45, -63, -54 }, { .5, .5, .5 });

    AddActorsToRenderer(_renderer, cone, realCone, cylinder);*/

    /*_polyLine = new PolyLine();

    _polyLine->SetCallback([this](PolyLine*) {
        renderWindow()->Render();
    });

    _renderer->AddActor(_polyLine->GetActor());*/

    ProbePartCatalog catalog("p/ProbePartCatalogue.xml");

    const auto extensions = catalog.Extensions();
    const auto modules = catalog.Modules();
    const auto probes = catalog.Probes();
    const auto styluses = catalog.Styluses();

    /*const auto part = catalog.FindByName("TP20_STD");
    const auto foundPartActor = GeneratePolyPart(part->Geometry());
    SetActorLightingPlastic(foundPartActor);
    foundPartActor->SetPosition(140, 0, 0);
    _renderer->AddActor(foundPartActor);*/

    /*ProbeToolAssembly toolFromAssembly;
    const auto assembly = catalog.FindAssembly(std::string("SP25xModulex20x2"));
    toolFromAssembly.Build(catalog, assembly);

    const auto toolModel = toolFromAssembly.Weld();

    toolModel->SetPosition(0, 0, 60);
    _renderer->AddActor(toolModel);*/

    ProbeToolAssembly manualTool;
    manualTool.Build(catalog, {"PAA2 x 140", "M2x90_CF", "TP20_EM2", "Eagle_p_touch_LF", "M2_20x1_TC"});

    const auto manualToolModel = manualTool.Weld();

    SetActorLightingPlastic(manualToolModel);

    _renderer->AddActor(manualToolModel);

    _renderer->ResetCamera();
    showGizmo();
}

vtkCamera* SceneWidget::camera() const {
    return _renderer->GetActiveCamera();
}

void SceneWidget::requestRender() const
{
    _renderTimer->start();
}

void SceneWidget::clear() const
{
    const auto actorCollection = _renderer->GetActors();

    auto lastActor = actorCollection->GetLastActor();
    if(lastActor) {
        _renderer->RemoveActor(lastActor);
    }
}

void SceneWidget::renderScene() {
    _renderer->GetRenderWindow()->Render();
}

void SceneWidget::prepareToRender(vtkObject *caller, ulong eventId, void *callData) {
    // empty
}

const vtkSmartPointer<vtkRenderer>& SceneWidget::getRenderer()
{
    return _renderer;
}

void SceneWidget::updateRendererCamera(vtkObject*, ulong, void*)
{
    _renderer->GetActiveCamera()->Modified();
}

void SceneWidget::showGizmo()
{
    vtkNew<vtkAxesActor> axes;
    axes->SetShaftTypeToCylinder();
    axes->SetXAxisLabelText("X");
    axes->SetYAxisLabelText("Y");
    axes->SetZAxisLabelText("Z");
    axes->SetTotalLength(1.0, 1.0, 1.0);
    axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());
    axes->SetConeRadius(1.025 * axes->GetConeRadius());
    axes->SetSphereRadius(1.5 * axes->GetSphereRadius());

    _gizmo = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    _gizmo->SetOrientationMarker(axes);
    _gizmo->SetInteractor(renderWindow()->GetInteractor());
    _gizmo->SetViewport(.8, .8, 1, 1);
    _gizmo->EnabledOn();
    _gizmo->InteractiveOn();
}
