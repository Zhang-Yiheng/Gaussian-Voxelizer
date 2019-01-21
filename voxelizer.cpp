#include "voxelizer.hpp"

bool convert_normal = false;
string input = "0", output = "0";
Coordinate space_lower_bound{0,0,0}, space_upper_bound{255,255,255}, number_of_voxels{255,255,255};

void process_command(int argc, char const *argv[])
{
    argc--; argv++;

    while (argc > 0)
    {
        if ((*argv)[0] == '-')
        {
            if (!strcmp(*argv, "-i"))
            {
                argv++; argc--;
                input = *argv;
            }
            else if (!strcmp(*argv, "-o"))
            {
                argv++; argc--;
                output = *argv;
            }
            else if (!strcmp(*argv, "-cn"))
            {
                convert_normal = true;
            }
            else if (!strcmp(*argv, "-sl"))
            {
                for(int i = 0; i < 3; i++)
                {
                    argv++; argc--;
                    auto coordinate = to_rational(*argv);
                    if(i == 0)
                        space_lower_bound.x = coordinate;
                    else if(i == 1)
                        space_lower_bound.y = coordinate;
                    else
                        space_lower_bound.z = coordinate;
                }
            }
            else if (!strcmp(*argv, "-su"))
            {
            	for(int i = 0; i < 3; i++)
                {
                    argv++; argc--;
                    auto coordinate = to_rational(*argv);
                    if(i == 0)
                        space_upper_bound.x = coordinate;
                    else if(i == 1)
                        space_upper_bound.y = coordinate;
                    else
                        space_upper_bound.z = coordinate;
                }
            }
            else if (!strcmp(*argv, "-n"))
            {
                argv++; argc--;
                number_of_voxels.x = stoi(*argv);
                argv++; argc--;
                number_of_voxels.y = stoi(*argv);
                argv++; argc--;
                number_of_voxels.z = stoi(*argv);
                if (!(number_of_voxels.x > 0 && number_of_voxels.y > 0 && number_of_voxels.z > 0))
                {
                    cerr<<"invalid voxel number\n";
                    exit(1);
                }
            }
        }
        argv++; argc--;
    }
}

rational<int> to_rational(const char str[])
{
    int length, numerator, denominator;
    double number;
    length = strlen(str);
    number = stod(str);
    numerator = (int)(number * pow(10, length));
    denominator = pow(10, length);
    rational<int> fraction(numerator, denominator);
    return fraction;
}

rational<int> to_rational(string str)
{
    int length, numerator, denominator;
    double number;
    length = str.size();
    number = stod(str);
    numerator = (int)(number * pow(10, length));
    denominator = pow(10, length);
    rational<int> fraction(numerator, denominator);
    return fraction;
}

rational<int> dot_product(Coordinate a, Coordinate b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Coordinate operator+(const Coordinate &a, const Coordinate &b)
{
    Coordinate p;
    p.x = a.x + b.x;
    p.y = a.y + b.y;
    p.z = a.z + b.z;
    return p;
}

Coordinate operator-(const Coordinate &a, const Coordinate &b)
{
    Coordinate p;
    p.x = a.x - b.x;
    p.y = a.y - b.y;
    p.z = a.z - b.z;
    return p;
}

Coordinate operator*(const Coordinate &a, const Coordinate &b)
{
    Coordinate p;
    p.x = a.y * b.z - a.z * b.y;
    p.y = a.z * b.x - a.x * b.z;
    p.z = a.x * b.y - a.y * b.x;
    return p;
}

void Mesh::read(string input)
{
    ifstream data(input);
    if (!data)
    {
        cerr << "can not open input file!\n";
        exit(1);
    }

    string str;
    getline(data,str);
    
    data >> str;
    while (str == "#")
    {
        getline(data, str);
        data >> str;
    }

    number_of_vertices = stoi(str);
    data >> str;
    number_of_faces = stoi(str);
    data >> str;
    number_of_edges = stoi(str);

    Coordinate a_vertex;
    data >> str;
    a_vertex.x = to_rational(str);
    lower_bound.x = upper_bound.x = a_vertex.x;
    data >> str;
    a_vertex.y = to_rational(str);
    lower_bound.y = upper_bound.y = a_vertex.y;
    data >> str;
    a_vertex.z = to_rational(str);
    lower_bound.z = upper_bound.z = a_vertex.z;

    vertex.push_back(a_vertex);

    for (int i = 1; i < number_of_vertices; i++)
    {
        data >> str;
        a_vertex.x = to_rational(str);
        if(a_vertex.x < lower_bound.x)
            lower_bound.x = a_vertex.x;
        else if(a_vertex.x > upper_bound.x)
            upper_bound.x = a_vertex.x;

        data >> str;
        a_vertex.y = to_rational(str);
        if(a_vertex.y < lower_bound.y)
            lower_bound.y = a_vertex.y;
        else if(a_vertex.y > upper_bound.y)
            upper_bound.y = a_vertex.y;

        data >> str;
        a_vertex.z = to_rational(str);
        if(a_vertex.z < lower_bound.z)
            lower_bound.z = a_vertex.z;
        else if(a_vertex.z > upper_bound.z)
            upper_bound.z = a_vertex.z;

        vertex.push_back(a_vertex);
    }

    while (lower_bound.x < space_lower_bound.x || lower_bound.y < space_lower_bound.y || lower_bound.z < space_lower_bound.z)
    {
        cout << "set space lower bound smaller than (" << lower_bound.x << ", " << lower_bound.y << ", " << lower_bound.z << ")\n-sl ";
        cin >> str;
        space_lower_bound.x = to_rational(str);
        cin >> str;
        space_lower_bound.y = to_rational(str);
        cin >> str;
        space_lower_bound.z = to_rational(str);
        cout << "new space lower bound set: (" << space_lower_bound.x << ", " << space_lower_bound.y << ", " << space_lower_bound.z << ")" << endl;
    }
    
    while(upper_bound.x > space_upper_bound.x || upper_bound.y > space_upper_bound.y || upper_bound.z > space_upper_bound.z)
    {
        cout << "set space upper bound bigger than (" << upper_bound.x << ", " << upper_bound.y << ", " << upper_bound.z << ")\n-su ";
        cin >> str;
        space_upper_bound.x = to_rational(str);
        cin >> str;
        space_upper_bound.y = to_rational(str);
        cin >> str;
        space_upper_bound.z = to_rational(str);
        cout << "new space upper bound set: (" << space_upper_bound.x << ", " << space_upper_bound.y << ", " << space_upper_bound.z << ")" << endl;
    }

    for(int i = 0; i < number_of_faces; i++)
    {
        data >> str;
        int vertices = stoi(str);
        
        if (vertices > 3)
            not_triangle_face.push_back(i);

        vector<int> a_face;
        for(int j = 0; j < vertices; j++) {
            data >> str;
            a_face.push_back(stoi(str));
        }

        face.push_back(a_face);

        auto vector1 = vertex[a_face[1]] - vertex[a_face[0]];
        auto vector2 = vertex[a_face[2]] - vertex[a_face[1]];
        face_normal.push_back(vector1*vector2);
    }
}

// void Mesh::translate_vertex()
// {

// }

// void Mesh::triangulate_face()
// {

// }

// void Mesh::convert_face_normal()
// {

// }

bool Mesh::intersectant(rational<int> x, rational<int> y, Coordinate &A, Coordinate &B, Coordinate &C)
{
    Coordinate P{x, y, 0};

    auto PA = A - P;
    auto PB = B - P;
    auto PC = C - P;

    auto PAXPB = PA * PB;
    auto PBXPC = PB * PC;
    auto PCXPA = PC * PA;

    if (PAXPB.z <= 0 && PBXPC.z <= 0 && PCXPA.z <= 0 || PAXPB.z >= 0 && PBXPC.z >= 0 && PCXPA.z >= 0)
        return true;
    
    return false;
}

void Mesh::insert_intersection_point(int i, int j, Intersection &point, int face_num)
{
    point.shared_face.push_back(face_num);

    if (intersection_points_along_ray[i][j].empty())
        intersection_points_along_ray[i][j].push_back(point);
    else
        for (auto pt = intersection_points_along_ray[i][j].begin(); pt != intersection_points_along_ray[i][j].end(); pt++)
        {
            if (point.value == (*pt).value)
            {
                (*pt).enter = ((*pt).enter || point.enter);
                (*pt).exit = ((*pt).exit || point.exit);
                (*pt).touch_start = ((*pt).touch_start || point.touch_start);
                (*pt).touch_end = ((*pt).touch_end || point.touch_end);
                (*pt).shared_face.push_back(face_num);
                break;
            }
            else if (point.value < (*pt).value)
            {
                intersection_points_along_ray[i][j].insert(pt, point);
                break;
            }
            if (pt == intersection_points_along_ray[i][j].end() - 1)
            {
                intersection_points_along_ray[i][j].push_back(point);
                break;
            }
        }
}

void Mesh::ray_triangle_intersection(Coordinate &space_lower_bound, Coordinate &voxel_size, int face_num)
{
    int v1 = face[face_num][0];
    int v2 = face[face_num][1];
    int v3 = face[face_num][2];
    Coordinate A{vertex[v1].x, vertex[v1].y, 0};
    Coordinate B{vertex[v2].x, vertex[v2].y, 0};
    Coordinate C{vertex[v3].x, vertex[v3].y, 0};

    // get triangle bounding box on x,y plane
    Coordinate triangle_lower_bound, triangle_upper_bound;
    triangle_lower_bound.x = triangle_upper_bound.x = A.x;
    triangle_lower_bound.y = triangle_upper_bound.y = A.y;

    if (B.x < triangle_lower_bound.x)
        triangle_lower_bound.x = B.x;
    else if (B.x > triangle_upper_bound.x)
        triangle_upper_bound.x = B.x;
    if (C.x < triangle_lower_bound.x)
        triangle_lower_bound.x = C.x;
    else if (C.x > triangle_upper_bound.x)
        triangle_upper_bound.x = C.x;
    if (B.y < triangle_lower_bound.y)
        triangle_lower_bound.y = B.y;
    else if (B.y > triangle_upper_bound.y)
        triangle_upper_bound.y = B.y;
    if (C.y < triangle_lower_bound.y)
        triangle_lower_bound.y = C.y;
    else if (C.y > triangle_upper_bound.y)
        triangle_upper_bound.y = C.y;
    // get triangle bounding box on x,y plane

    int times_in_x = rational_cast<int>((triangle_lower_bound.x - space_lower_bound.x) / voxel_size.x);
    auto ray_min_x = space_lower_bound.x + voxel_size.x * times_in_x;
    int times_in_y = rational_cast<int>((triangle_lower_bound.y - space_lower_bound.y) / voxel_size.y);
    auto ray_min_y = space_lower_bound.y + voxel_size.y * times_in_y;

    auto ray_dot_product_face_normal = dot_product(ray, face_normal[face_num]);
    auto triangle_formula_d = face_normal[face_num].x * vertex[v1].x + face_normal[face_num].y * vertex[v1].y + face_normal[face_num].z * vertex[v1].z;
    Intersection intersection_point;

    int ray_i = times_in_x;
    for (auto x = ray_min_x; x <= triangle_upper_bound.x; x += voxel_size.x, ray_i++)
    {
        int ray_j = times_in_y;
        for (auto y = ray_min_y; y <= triangle_upper_bound.y; y += voxel_size.y, ray_j++)
        {
            if (intersectant(x, y, A, B, C))
            {
                if (ray_dot_product_face_normal < 0)
                {
                    intersection_point.enter = true;
                    intersection_point.value = (triangle_formula_d - face_normal[face_num].x * x - face_normal[face_num].y * y) / face_normal[face_num].z;
                    insert_intersection_point(ray_i, ray_j, intersection_point, face_num);
                }
            }
            
        }
    }
    
}

void Mesh::voxelize(Coordinate &space_lower_bound, Coordinate &space_upper_bound, Coordinate &number_of_voxels, bool convert_normal)
{
    // if (!not_triangle_face.empty())
    //     triangulate_face();

    // if (convert_normal)
    //     convert_face_normal();

    intersection_points_along_ray.resize(number_of_voxels.x);
    for (int i = 0; i < number_of_voxels.x; i++) {
        intersection_points_along_ray[i].resize(number_of_voxels.y);
    }
    

    Coordinate voxel_size;
    voxel_size.x = (space_upper_bound.x - space_lower_bound.x) / number_of_voxels.x;
    voxel_size.y = (space_upper_bound.y - space_lower_bound.y) / number_of_voxels.y;
    voxel_size.z = (space_upper_bound.z - space_lower_bound.z) / number_of_voxels.z;

    for (int face_num = 0; face_num < number_of_faces; face_num++) {
        ray_triangle_intersection(space_lower_bound, voxel_size, face_num);
    }
    
    
}