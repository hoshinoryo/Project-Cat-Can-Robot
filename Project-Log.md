# ***PROJECT FRAME***

Main
  ↓
Manager
  ↓
Scene
  ├─ Title
  ├─ Game
  │   ├─ Bg
  │   ├─ Block
  │   └─ Player
  └─ Result



#### 3D Shader

VS b0 world
VS b1 view
VS b2 proj
VS/PS b5 shadow

PS b0 diffuse_color
PS b1 ambient_color
PS b2 directional light
PS b3 specular
PS b4 point light



Renderer_Manager
DirectX共通资源
device / context / swapchain / render target / depth / BeginFrame / EndFrame

Renderer3D
3D通用资源
VERTEX_3D定义 / view projection buffer / 光照buffer / 3D基础状态

ModelShader
CPU侧shader管理
读取cso / 创建VS PS InputLayout / 创建shader用constant buffer / Begin / SetWorldMatrix / SetColor

ModelRenderer
模型绘制
绑定mesh的VB IB / 找材质 / 找贴图 / 调用ModelShader / DrawIndexed

ModelAsset
模型资源
Assimp读取 / mesh buffer / material / texture / nodeToModel



# ***UPDATE LOG***