g++  -c ../*.cpp ../glad.c  -I../include -I../dependencies/glfw/include  -I../dependencies/glad/include -I../dependencies/ -I../dependencies/stb/  -L../dependencies/glfw/lib -lglfw3 -lmingw32 -lopengl32 -lgdi32

ar rcs libdefengine.a *.o