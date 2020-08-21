#define _CRT_SECURE_NO_WARNINGS
#define PAR_SHAPES_IMPLEMENTATION
#include "headers/Utils.h"

Utils::Utils() {
	Image im = LoadImage("src/Assets/numbers.png");
	for (int i = 0; i < 10; i++) {
		numbers[i] = LoadTextureFromImage(ImageFromImage(im, { (float)i * 128, 0, 128, 128 }));
	}
}

Mesh Utils::GenRock() {
	Mesh mesh = { 0 };
	mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));
	
	par_shapes_mesh *rock = par_shapes_create_rock(GetRandomValue(1, 100), 1);
	par_shapes_scale(rock, 0.5f, 1.0f, 0.5f);

	mesh.vertices = (float *)RL_MALLOC(rock->ntriangles * 3 * 3 * sizeof(float));
	mesh.texcoords = (float *)RL_MALLOC(rock->ntriangles * 3 * 2 * sizeof(float));
	mesh.normals = (float *)RL_MALLOC(rock->ntriangles * 3 * 3 * sizeof(float));

	mesh.vertexCount = rock->ntriangles * 3;
	mesh.triangleCount = rock->ntriangles;

	

	for (int k = 0; k < mesh.vertexCount; k++)
	{
		mesh.vertices[k * 3] = rock->points[rock->triangles[k] * 3];
		mesh.vertices[k * 3 + 1] = rock->points[rock->triangles[k] * 3 + 1];
		mesh.vertices[k * 3 + 2] = rock->points[rock->triangles[k] * 3 + 2];

		mesh.normals[k * 3] = rock->normals[rock->triangles[k] * 3];
		mesh.normals[k * 3 + 1] = rock->normals[rock->triangles[k] * 3 + 1];
		mesh.normals[k * 3 + 2] = rock->normals[rock->triangles[k] * 3 + 2];

		mesh.texcoords[k * 2] = 1.0f;
		mesh.texcoords[k * 2 + 1] = 1.0f;
	}

	par_shapes_free_mesh(rock);
	
	// Upload vertex data to GPU (static mesh)
	rlLoadMesh(&mesh, false);


	return mesh;
}

Mesh Utils::GenWall() {
	Image im = GenImagePerlinNoise(64, 64, 4, 8, GetRandomValue(1, 2) * 0.2);
	Mesh m = GenMeshHeightmap(im, { 1.0f, 0.4f, 1.0f });
	return m;
}

Mesh Utils::GenBall() {
	Mesh m = GenMeshSphere(0.5f, 10, 10);
	return m;
}

Mesh Utils::GenGem(int size) {
	Mesh mesh = { 0 };
	mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));
	par_shapes_mesh *gem;
	switch (size) {
		case SMALL: gem = par_shapes_create_octahedron();
					break;
		case MEDIUM: gem = par_shapes_create_dodecahedron();
					 break;
		case LARGE: gem = par_shapes_create_icosahedron();
					break;
		default: gem = par_shapes_create_octahedron();
				 break;
	}
	par_shapes_scale(gem, 0.5f, 0.5f, 0.5f);
	gem->tcoords = PAR_MALLOC(float, 2 * gem->npoints);
	par_shapes_compute_normals(gem);

	mesh.vertices = (float *)RL_MALLOC(gem->ntriangles * 3 * 3 * sizeof(float));
	mesh.texcoords = (float *)RL_MALLOC(gem->ntriangles * 3 * 2 * sizeof(float));
	mesh.normals = (float *)RL_MALLOC(gem->ntriangles * 3 * 3 * sizeof(float));

	mesh.vertexCount = gem->ntriangles * 3;
	mesh.triangleCount = gem->ntriangles;

	for (int k = 0; k < mesh.vertexCount; k++)
	{
		mesh.vertices[k * 3] = gem->points[gem->triangles[k] * 3];
		mesh.vertices[k * 3 + 1] = gem->points[gem->triangles[k] * 3 + 1];
		mesh.vertices[k * 3 + 2] = gem->points[gem->triangles[k] * 3 + 2];

		mesh.normals[k * 3] = gem->normals[gem->triangles[k] * 3];
		mesh.normals[k * 3 + 1] = gem->normals[gem->triangles[k] * 3 + 1];
		mesh.normals[k * 3 + 2] = gem->normals[gem->triangles[k] * 3 + 2];

		mesh.texcoords[k * 2] = gem->tcoords[gem->triangles[k] * 2];
		mesh.texcoords[k * 2 + 1] = gem->tcoords[gem->triangles[k] * 2 + 1];
	}

	par_shapes_free_mesh(gem);
	rlLoadMesh(&mesh, false);

	return mesh;
}

Color Utils::RequestGemColor() {
	int n = GetRandomValue(0, 8);
	Color c;
	switch (n){
	case 0: c = RED; break;
	case 1: c = BLUE; break;
	case 2: c = SKYBLUE; break;
	case 3: c = GREEN; break;
	case 4: c = PINK; break;
	case 5: c = MAROON; break;
	case 6: c = PURPLE; break;
	case 7: c = VIOLET; break;
	case 8: c = ORANGE; break;
	}
	c.a = 200;
	return c;
}

Mesh Utils::GenRocket() {
	Mesh mesh = { 0 };
	mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));
	par_shapes_mesh *body = par_shapes_create_cylinder(5, 5);
	par_shapes_scale(body, 1.0f, 1.0f, 3.0f);
	par_shapes_translate(body, 0.0f, 0.0f, 0.0f);
	par_shapes_mesh *top = par_shapes_create_cone(10, 10);
	par_shapes_scale(top, 1.5f, 1.5f, 2.0f);
	par_shapes_translate(top, 0.0f, 0.0f, 2.5f);
	par_shapes_merge(top, body);

	par_shapes_scale(top, 0.3f, 0.3f, 0.3f);

	mesh.vertices = (float *)RL_MALLOC(top->ntriangles * 3 * 3 * sizeof(float));
	mesh.texcoords = (float *)RL_MALLOC(top->ntriangles * 3 * 2 * sizeof(float));
	mesh.normals = (float *)RL_MALLOC(top->ntriangles * 3 * 3 * sizeof(float));

	mesh.vertexCount = top->ntriangles * 3;
	mesh.triangleCount = top->ntriangles;

	for (int k = 0; k < mesh.vertexCount; k++)
	{
		mesh.vertices[k * 3] = top->points[top->triangles[k] * 3];
		mesh.vertices[k * 3 + 1] = top->points[top->triangles[k] * 3 + 1];
		mesh.vertices[k * 3 + 2] = top->points[top->triangles[k] * 3 + 2];

		mesh.normals[k * 3] = top->normals[top->triangles[k] * 3];
		mesh.normals[k * 3 + 1] = top->normals[top->triangles[k] * 3 + 1];
		mesh.normals[k * 3 + 2] = top->normals[top->triangles[k] * 3 + 2];

		mesh.texcoords[k * 2] = top->tcoords[top->triangles[k] * 2];
		mesh.texcoords[k * 2 + 1] = top->tcoords[top->triangles[k] * 2 + 1];
	}

	par_shapes_free_mesh(top);
	par_shapes_free_mesh(body);
	rlLoadMesh(&mesh, false);

	return mesh;
}

Mesh Utils::GenDynamite() {
	Mesh mesh = { 0 };
	mesh.vboId = (unsigned int *)RL_CALLOC(7, sizeof(unsigned int));
	par_shapes_mesh *part1 = par_shapes_create_cylinder(10, 10);
	par_shapes_mesh *part2 = par_shapes_create_cylinder(10, 10);
	par_shapes_mesh *part3 = par_shapes_create_cylinder(10, 10);
	par_shapes_translate(part2, 1.0f, 1.0f, 0.0f);
	par_shapes_translate(part3, -1.0f, 1.0f, 0.0f);
	par_shapes_merge(part1, part2);
	par_shapes_merge(part1, part3);

	mesh.vertices = (float *)RL_MALLOC(part1->ntriangles * 3 * 3 * sizeof(float));
	mesh.texcoords = (float *)RL_MALLOC(part1->ntriangles * 3 * 2 * sizeof(float));
	mesh.normals = (float *)RL_MALLOC(part1->ntriangles * 3 * 3 * sizeof(float));

	mesh.vertexCount = part1->ntriangles * 3;
	mesh.triangleCount = part1->ntriangles;

	for (int k = 0; k < mesh.vertexCount; k++)
	{
		mesh.vertices[k * 3] = part1->points[part1->triangles[k] * 3];
		mesh.vertices[k * 3 + 1] = part1->points[part1->triangles[k] * 3 + 1];
		mesh.vertices[k * 3 + 2] = part1->points[part1->triangles[k] * 3 + 2];

		mesh.normals[k * 3] = part1->normals[part1->triangles[k] * 3];
		mesh.normals[k * 3 + 1] = part1->normals[part1->triangles[k] * 3 + 1];
		mesh.normals[k * 3 + 2] = part1->normals[part1->triangles[k] * 3 + 2];

		mesh.texcoords[k * 2] = part1->tcoords[part1->triangles[k] * 2];
		mesh.texcoords[k * 2 + 1] = part1->tcoords[part1->triangles[k] * 2 + 1];
	}

	par_shapes_free_mesh(part1);
	par_shapes_free_mesh(part2);
	par_shapes_free_mesh(part3);

	rlLoadMesh(&mesh, false);

	return mesh;
}

void Utils::DrawPlaneTexture(Texture texture, Vector3 centerPos, Vector2 size, Color color) {
	if (rlCheckBufferLimit(4)) rlglDraw();
	rlEnableTexture(texture.id);
	rlPushMatrix();
	rlTranslatef(centerPos.x, centerPos.y, centerPos.z);
	rlScalef(size.x, 1.0f, size.y);

	rlBegin(RL_QUADS);
		rlColor4ub(color.r, color.g, color.b, color.a);
		rlNormal3f(0.0f, 1.0f, 0.0f);
		rlTexCoord2f(1.0f, 1.0f); rlVertex3f(-0.5f, 0.0f, -0.5f);
		rlTexCoord2f(1.0f, 0.0f); rlVertex3f(-0.5f, 0.0f, 0.5f);
		rlTexCoord2f(0.0f, 0.0f); rlVertex3f(0.5f, 0.0f, 0.5f);
		rlTexCoord2f(0.0f, 1.0f); rlVertex3f(0.5f, 0.0f, -0.5f);
	rlEnd();
	rlPopMatrix();
	rlDisableTexture();
}


int length(int number){
	int counter = 0;
	while (number)
	{
		number = number / 10;
		counter++;
	}
	return (counter);
}

void Utils::DrawScore(unsigned int score, float x, float y) {
	int len = length(score);
	for (int i = 0; i < len; i++) {
		DrawPlaneTexture(numbers[score % 10], { x + i, 0.01f, y }, { 1.0f, 1.0f }, WHITE);
		score /= 10;
	}
}

void Utils::ShakeCamera(Camera &camera) {
	camera.position.x += GetRandomValue(-5, 5) * GetFrameTime();
	camera.position.z += GetRandomValue(-5, 5) * GetFrameTime();
	camera.target.x += GetRandomValue(-5, 5) * GetFrameTime();
	camera.target.z += GetRandomValue(-5, 5) * GetFrameTime();
}