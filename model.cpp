#include "model.hpp"

void line(HDC hdc, int x0, int y0, int x1, int y1, const COLORREF &color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        SetPixel(hdc, (steep?y:x), (steep?x:y), color);
        error2 += derror2;

        if (error2 > dx) {
            y += (y1>y0?1:-1);
            error2 -= dx*2;
        }
    }
}

void triangle(HDC hdc, Vec2i t0, Vec2i t1, Vec2i t2, const COLORREF &color){
    if (t0.y == t1.y && t0.y == t2.y) return;

    Vec2i Min = t0, Max = t0;
    Min.x = t1.x < Min.x? t1.x: Min.x; Max.x = t1.x > Max.x? t1.x: Max.x;
    Min.y = t2.y < Min.y? t2.y: Min.y; Max.y = t2.y > Max.y? t2.y: Max.y;
    line(hdc, Min.x, Min.y, Max.x, Min.y, RGB(255, 0, 0));
    line(hdc, Min.x, Max.y, Max.x, Max.y, RGB(255, 0, 0));
    line(hdc, Min.x, Min.y, Min.x, Max.y, RGB(255, 0, 0));
    line(hdc, Max.x, Min.y, Max.x, Max.y, RGB(255, 0, 0));

    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; ++i) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        double alpha = (double) i / total_height;
        double beta  = (double)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
        Vec2i A =               t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
        if (A.x > B.x) std::swap(A, B);
        for(int j = A.x; j <= B.x; ++j){
            SetPixel(hdc, j, t0.y + i, color);
        }
    }
}

Model::Model(const char *filename) : verts(), faces() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    std::string trash;
    while (!in.eof()) {
        std::getline(in, line);
        std::stringstream iss(line);
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3d v;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            v.x = std::stod(data[0]);
            v.y = std::stod(data[1]);
            v.z = std::stod(data[2]);
            if(!verts.size()){
                Min = Max = v;
            }else{
                Min.x = v.x < Min.x ? v.x: Min.x;
                Min.y = v.y < Min.y ? v.y: Min.y;
                Min.z = v.z < Min.z ? v.z: Min.z;
                Max.x = v.x > Max.x ? v.x: Max.x;
                Max.y = v.y > Max.y ? v.y: Max.y;
                Max.z = v.z > Max.z ? v.z: Max.z;
            }
            verts.push_back(v);
        }
        if (!line.compare(0, 2, "f ")) {
            iss >> trash;
            std::vector<int> f;
            std::string data[3];
            for(int i = 0; i < 3; ++i) {
                iss >> data[i];
                data[i].erase(data[i].find('/'), data[i].length() - 1);
                f.push_back(std::stoi(data[i]) - 1);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v- " << verts.size() << " f- "  << faces.size() << std::endl;
    std::cerr << "Min - " << Min.x << ' ' << Min.y << ' ' << Min.z << std::endl;
    std::cerr << "Max - " << Max.x << ' ' << Max.y << ' ' << Max.z << std::endl;
}

Model::~Model() {
}

int Model::getCantidadVerts() {
    return (int)verts.size();
}

int Model::getCantidadFaces() {
    return (int)faces.size();
}

std::vector<int> Model::getFace(int id) {
    return faces[id];
}

Vec3d Model::getVert(int i) {
    return verts[i];
}

void Model::drawMesh(HDC hdc, int size, const COLORREF &color){
    double x = Max.x - Min.x;
    double y = Max.y - Min.y;
    double scale = size / (x > y ? x: y);
    double dx = - Min.x;
    double dy = - Min.y;
    for(unsigned int i = 0; i < faces.size(); ++i){
    for(int j = 0; j < 3; ++j){
        Vec3d v0 = verts[faces[i][j]];
        Vec3d v1 = verts[faces[i][(j + 1) % 3]];
        int x0 = (v0.x + dx) * scale;
        int x1 = size - (v0.y + dy) * scale;
        int y0 = (v1.x + dx) * scale;
        int y1 = size - (v1.y + dy) * scale;
        line(hdc, x0, x1, y0, y1, color);
        } 
    }
}

void Model::drawMeshTriangle(HDC hdc, int size, const COLORREF &color){
    double x = Max.x - Min.x;
    double y = Max.y - Min.y;
    double scale = size / (x > y ? x: y);
    double dx = - Min.x;
    double dy = - Min.y;
    for (unsigned int i = 0; i < faces.size(); ++i) {
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            Vec3d world_coords = verts[faces[i][j]];
            screen_coords[j] = Vec2i((world_coords.x + dx) * scale, size - ((world_coords.y + dy) * scale));
        }
        triangle(hdc, screen_coords[0], screen_coords[1], screen_coords[2], RGB(rand()%255, rand()%255, rand()%255));
    }
}