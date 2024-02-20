#pragma once
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QTimer>

#include <QVTKOpenGLNativeWidget.h>
#include <QColorDialog>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
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

#include <Eigen/Core>
#include <Eigen/Dense>

#include <algorithm>