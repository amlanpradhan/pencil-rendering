
program:
	g++ main.cpp Texture.cpp Shader.cpp bmp.cpp fbo.cpp canny.cpp trimesh2/TriMesh_curvature.cc trimesh2/TriMesh_grid.cc trimesh2/TriMesh_io.cc trimesh2/conn_comps.cc trimesh2/diffuse.cc trimesh2/edgeflip.cc trimesh2/faceflip.cc trimesh2/filter.cc trimesh2/GLCamera.cc trimesh2/ICP.cc trimesh2/KDtree.cc trimesh2/lmsmooth.cc trimesh2/overlap.cc trimesh2/remove.cc trimesh2/reorder_verts.cc trimesh2/shared.cc trimesh2/subdiv.cc trimesh2/TriMesh_bounding.cc trimesh2/TriMesh_connectivity.cc trimesh2/TriMesh_normals.cc trimesh2/TriMesh_pointareas.cc trimesh2/TriMesh_stats.cc trimesh2/TriMesh_tstrips.cc -m32 -framework openGl -framework glut -o output

clean:
	rm output
