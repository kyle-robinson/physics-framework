#pragma once
#ifndef STDAFX_H
#define STDAFX_H

#include <memory>
#include <algorithm>

// Data Structures
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <map>

// Windows API
#include <Windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

// Classes
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Logging/ErrorLogger.h"
#include "../Graphics/Objects/Data/Vertex.h"

#endif