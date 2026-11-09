import sys
import math

def create_icosphere(subdivisions=0, filename="icosphere.obj"):
    # Golden ratio
    t = (1.0 + math.sqrt(5.0)) / 2.0
    
    # 12 base vertices of an icosahedron
    vertices = [
        [-1,  t,  0], [ 1,  t,  0], [-1, -t,  0], [ 1, -t,  0],
        [ 0, -1,  t], [ 0,  1,  t], [ 0, -1, -t], [ 0,  1, -t],
        [ t,  0, -1], [ t,  0,  1], [-t,  0, -1], [-t,  0,  1]
    ]
    
    # Normalize base vertices
    for i in range(len(vertices)):
        length = math.sqrt(sum(v**2 for v in vertices[i]))
        vertices[i] = [v / length for v in vertices[i]]
        
    # 20 base faces
    faces = [
        (0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11),
        (1, 5, 9), (5, 11, 4), (11, 10, 2), (10, 7, 6), (7, 1, 8),
        (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8), (3, 8, 9),
        (4, 9, 5), (2, 4, 11), (6, 2, 10), (8, 6, 7), (9, 8, 1)
    ]
    
    # Vertex cache to avoid duplicates during subdivision
    middle_point_cache = {}
    
    def get_middle_point(p1, p2):
        key = tuple(sorted((p1, p2)))
        if key in middle_point_cache:
            return middle_point_cache[key]
        
        v1, v2 = vertices[p1], vertices[p2]
        middle = [(v1[i] + v2[i]) / 2.0 for i in range(3)]
        
        # Snap to sphere surface
        length = math.sqrt(sum(m**2 for m in middle))
        vertices.append([m / length for m in middle])
        
        index = len(vertices) - 1
        middle_point_cache[key] = index
        return index

    # Subdivide faces
    for _ in range(subdivisions):
        new_faces = []
        for tri in faces:
            a = get_middle_point(tri[0], tri[1])
            b = get_middle_point(tri[1], tri[2])
            c = get_middle_point(tri[2], tri[0])
            
            new_faces.extend([
                (tri[0], a, c),
                (tri[1], b, a),
                (tri[2], c, b),
                (a, b, c)
            ])
        faces = new_faces

    # Write out OBJ file format
    with open(filename, "w") as f:
        f.write("o ico\n")
        for v in vertices:
            f.write(f"v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n")
        for v in vertices:
            f.write(f"vn {-v[0]:.6f} {-v[1]:.6f} {-v[2]:.6f}\n")
        for tri in faces:
            # OBJ is 1-indexed
            f.write(f"f {tri[0]+1}/0/{tri[0]+1} {tri[1]+1}/0/{tri[1]+1} {tri[2]+1}/0/{tri[2]+1}\n")

# Run this script to generate your file
create_icosphere(subdivisions=int(sys.argv[1]), filename="ico.obj")
print("File generated successfully!")