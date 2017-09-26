#include "renderer.h"

Renderer::Renderer()
{

}

void Renderer::SetObjectToWorldMatrix( const Matrix4f& objectMatrix )
{
    objMatrix = objectMatrix;
    objViewMatrix = viewMatrix * objMatrix;
}
void Renderer::SetWorldToViewMatrix( const Matrix4f& viewMatrix )
{
    this->viewMatrix = viewMatrix;
    objViewMatrix = this->viewMatrix * objMatrix;
}
void Renderer::SetViewToPerspectiveMatrix( const Matrix4f& perspectiveMatrix )
{
    perspMatrix = perspectiveMatrix;
    perspScreenMatrix = screenMatrix * perspMatrix;
}
void Renderer::SetViewToPerspectiveMatrix( const float &fov, const Uint16 &width, const Uint16 &height,
                                                  const float &zNear, const float &zFar )
{
    Matrix4f matrix = Matrix4f::perspectiveTransform( fov, (float) width / (float) height, zNear, zFar );
    SetViewToPerspectiveMatrix( matrix );
}
void Renderer::SetPerspectiveToScreenSpaceMatrix( const Matrix4f& screenspaceMatrix )
{
    screenMatrix = screenspaceMatrix;
    perspScreenMatrix = screenMatrix * perspMatrix;
}
