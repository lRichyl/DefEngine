g++  -O2 -c ../*.cpp ../glad.cpp  -I../include -I../dependencies/glfw/include  -I../dependencies/glad/include -I../dependencies/ -I../dependencies/stb/ -I../dependencies/OpenAL/include  -L../dependencies/glfw/lib -L../dependencies/OpenAL/lib -lglfw3 -lopengl32 -lOpenAL32 -lgdi32

ar rcs libdefengine.a *.o
