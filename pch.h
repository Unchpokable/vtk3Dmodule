#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <QMainWindow>
#include <QOpenGLWidget>
#include <QTimer>
#include <QDomDocument>
#include <QVTKOpenGLNativeWidget.h>
#include <QColorDialog>
#include <QFile>
#include <QMetaEnum>
#include <QPixmap>
#include <QTableWidget>
#include <QHeaderView>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkConeSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkCylinderSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkFollower.h>
#include <vtkLineSource.h>
#include <vtkTexture.h>
#include <vtkPNGReader.h>
#include <vtkImageFlip.h>
#include <vtkPlaneSource.h>
#include <vtkCallbackCommand.h>
#include <vtkCellData.h>
#include <vtkLine.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyLine.h>
#include <vtkAssembly.h>
#include <vtkPolygon.h>
#include <vtkOBJImporter.h>
#include <vtkOBJReader.h>
#include <vtkAppendPolyData.h>
#include <vtkPointData.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSubdivisionFilter.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkQuadricClustering.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataNormals.h>
#include <vtkMatrix4x4.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkQWidgetTexture.h>
#include <vtkQWidgetRepresentation.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <algorithm>
#include <filesystem>