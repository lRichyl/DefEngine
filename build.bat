g++ -g -O0 -w -Wl,-subsystem,console *.cpp  -o EngineTest.exe -Iinclude -Idependencies/glfw/include -Idependencies/OpenAL/include  -Idependencies/glad/include -Idependencies/ -Idependencies/stb/  -Ldependencies/glfw/lib -Ldependencies/OpenAL/lib -lglfw3 -lmingw32 -lOpenAL32 -lopengl32 -lgdi32