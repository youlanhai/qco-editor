
#ifdef GL_ES
varying mediump vec2 TextureCoordOut;
#else
varying vec2 TextureCoordOut;
#endif

uniform vec4 u_color;

void main(void)
{
    vec4 color = texture2D(CC_Texture0, TextureCoordOut) * u_color;
    if(color.a < 0.001)
    {
        discard;
    }
    gl_FragColor = color;
}
