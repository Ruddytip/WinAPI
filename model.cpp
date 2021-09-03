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




    line(hdc, t0.x, t0.y, t1.x, t1.y, RGB(255, 0, 0));
    line(hdc, t1.x, t1.y, t2.x, t2.y, RGB(0, 255, 0));
    line(hdc, t2.x, t2.y, t0.x, t0.y, RGB(0, 255, 0));
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
        if (line.compare(0, 2, "v ") == 0) {
            iss >> trash;
            Vec3d v;
            std::string data[3];
            for(int i = 0; i < 3; ++i) iss >> data[i];
            v.x = std::stod(data[0]);
            v.y = std::stod(data[1]);
            v.z = std::stod(data[2]);
            verts.push_back(v);
        }
        if (line.compare(0, 2, "f ") == 0) {
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
    double scale = 0.4;
    double dx = size / 2.;
    double dy = 0.0;
    for(unsigned int i = 0; i < faces.size(); ++i){
    for(int j = 0; j < 3; ++j){
        Vec3d v0 = verts[faces[i][j]];
        Vec3d v1 = verts[faces[i][(j + 1) % 3]];
        int x0 = (v0.x + 1.) * scale + dx;
        int x1 = size - ((v0.y + 1.) * scale) + dy;
        int y0 = (v1.x + 1.) * scale + dx;
        int y1 = size - ((v1.y + 1.) * scale) + dy;
        line(hdc, x0, x1, y0, y1, RGB(255, 255, 255));
        } 
    }
}
