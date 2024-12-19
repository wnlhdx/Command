#version 450
    
    layout(location = 0) out vec3 fragColor;
    
    vec3 positions[3] = vec3[](
      //vec3(0.0, -0.5,0.0),        //三角形上面的顶点      
      vec3(0.5, 0.5,0.0),        //右边的点
      vec3(-0.5, 0.5,0.0)        //左边的点
    );
    
    vec3 colors[3] = vec3[](
      vec3(1.0, 0.0, 0.0),    //红色
      vec3(0.0, 1.0, 0.0),    //绿色
      vec3(0.0, 0.0, 1.0)    //蓝色
    );
    
    void main() {
        gl_Position = vec4(positions[gl_VertexIndex], 1.0);
        fragColor = colors[gl_VertexIndex];
    }