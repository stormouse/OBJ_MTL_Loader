#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <list>
#include "cbmp.h"

struct Vector3
{
	double x, y, z;
};

struct UV
{
	double u, v;
};

struct Texture
{
	GLuint id;
	int height;
	int width;
	BMP *bmp;
	struct Texture(const char *filename);
};

Texture::Texture(const char *filename)
{
	bmp = new BMP();
	if (!LoadBmp(*bmp, filename))
	{
		printf("bmp load failed\n");
	}
	height = bmp->bmHeight;
	width = bmp->bmWidth;

	GLuint formerUsedTex;
	glGetIntegerv(GL_TEXTURE_2D, (GLint*)&formerUsedTex);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glGenTextures(1, &id);
#ifdef DEBUG
		printf("texture id:%u using image %s\n", id, filename);
#endif
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->bmWidth, bmp->bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp->bmBits);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, formerUsedTex);
	glPopAttrib();

#ifdef DEBUG
	printf("BMP width:%d, height:%d\n", bmp->bmWidth, bmp->bmHeight);
#endif

	delete bmp;
}

struct Material
{
	double Ns;
	double Ka[3];
	double Kd[3];
	double Ks[3];
	double Ni;
	double d;
	double Tr;		//for transparency
	int illum;

	char map_Kd[256];
	char map_Ka[256];
	char map_Ks[256];
	char map_d[256];
	char map_Ns[256];
	char map_bump[256];

	Texture *t_Kd;
	Texture *t_Ka;
	Texture *t_Ks;
	Texture *t_d;
	Texture *t_Ns;
	Texture *t_bump;

	char name[256];
};

struct Triangle
{
	int Vertex[3];
	int Normal[3];
	int	UV_Pnt[3];
	Material *mat;
};

typedef struct
{
	char *name;
	int p;
}GroupBound, ObjectBound;

class OBJModel
{
private:
	int nVertex = 0;
	int nNormal = 0;
	int nVertUV = 0;
	int *groups;
	int *objects;
	int groupCount = 0, objectCount = 0;

	std::list<Vector3>		vertexList;
	std::list<Vector3>		normalList;
	std::list<UV>			uvList;
	std::list<Triangle>		triangleList;
	std::list<ObjectBound>	objectBounds;
	std::list<GroupBound>	groupBounds;
	std::list<Material*>	matList;
	Vector3*				vertexArray;
	Vector3*				normalArray;
	UV*						uvArray;
	Material*				currentMat;
	Material*				mat = NULL;		//temp mat for add procedure
	void readString(const char *str);
	Material* findMaterial(const char *name) const;
	void readMTL(const char *filename);
	void readMTLString(const char *str);

public:
	OBJModel(const char *filename);
	void Draw(int mode = 0);
};

OBJModel::OBJModel(const char *filename)
{
	currentMat = NULL;
	// create 0 bound;
	ObjectBound e; e.name = NULL; e.p = 0;
	objectBounds.push_back(e);
	groupBounds.push_back(e);

	FILE *fp;
	fopen_s(&fp, filename, "r");
	if (fp == NULL)
	{
		puts("failed to open obj file\n");
		exit(1);
	}
	char buffer[1024];
	while(fgets(buffer, 1024, fp))
		readString(buffer);

	fclose(fp);

#ifdef DEBUG
	printf("nVertex : %d\nnNormal : %d\nnUV : %d\n", nVertex, nNormal, nVertUV);
#endif

	vertexArray = new Vector3[nVertex];
	normalArray = new Vector3[nNormal];
	uvArray = new UV[nVertUV];
	groups = new int[groupCount + 1];
	objects = new int[objectCount + 1];

	int i = 0;
	for (std::list<Vector3>::const_iterator it = vertexList.begin(); it != vertexList.end(); it++)
	{
		vertexArray[i++] = *it;
	}

	i = 0;
	for (std::list<Vector3>::const_iterator it = normalList.begin(); it != normalList.end(); it++)
		normalArray[i++] = *it;

	i = 0;
	for (std::list<UV>::const_iterator it = uvList.begin(); it != uvList.end(); it++)
		uvArray[i++] = *it;

	i = 0;
	for (std::list<GroupBound>::const_iterator it = groupBounds.begin(); it != groupBounds.end(); it++)
		groups[i++] = (*it).p;

	i = 0;
	for (std::list<ObjectBound>::const_iterator it = objectBounds.begin(); it != objectBounds.end(); it++)
		objects[i++] = (*it).p;

}

// read line
void OBJModel::readString(const char *str)
{
	char cmd[256]; 
	int i;
	for (i = 0; str[i] && str[i] != ' ' && i < 10; i++)
		cmd[i] = str[i];
	if(str[i] && i<8)cmd[i] = 0;
	else return;

	
#ifdef DEBUG
	printf("Reading %s\n", str);
#endif
	//vertex x y z
	if (_strcmpi(cmd, "v") == 0)
	{
		Vector3 v;
		sscanf_s(str + i + 1, "%lf %lf %lf", &v.x, &v.y, &v.z);
		vertexList.push_back(v);
		nVertex++;
	}
	//normal x y z
	else if (_strcmpi(cmd, "vn") == 0)
	{	
		Vector3 vn;
		sscanf_s(str + i + 1, "%lf %lf %lf", &vn.x, &vn.y, &vn.z);
		normalList.push_back(vn);
		nNormal++;
	}
	//uv_vertex u v
	else if (_strcmpi(cmd, "vt") == 0)
	{
		UV vt;
		sscanf_s(str + i + 1, "%lf %lf", &vt.u, &vt.v);
		uvList.push_back(vt);
		nVertUV++;
	}
	//face v/t/n v/t/n v/t/n
	else if (_strcmpi(cmd, "f") == 0)
	{
		int token[3], i;
		for (i = 0; str[i] != ' '; i++);
		token[0] = i + 1;
		for (i+=1; str[i] != ' '; i++);
		token[1] = i + 1;
		for (i+=1; str[i] != ' '; i++);
		token[2] = i + 1;
		
		Triangle t;
		for (int j = 0; j < 3; j++)
		{
			t.UV_Pnt[j] = t.Normal[j] = -1;
			sscanf_s(str+token[j], "%d/%d/%d", &t.Vertex[j], &t.UV_Pnt[j], &t.Normal[j]);
		}

		t.mat = currentMat;
#ifdef DEBUG
		printf("Face <%d-%d-%d>\n", t.Vertex[0], t.Vertex[1], t.Vertex[2]);
		if (t.mat != NULL)
			printf("Material : %s\n", t.mat->name);
#endif
		triangleList.push_back(t);
	}
	//object
	else if (_strcmpi(cmd, "o") == 0)
	{
		ObjectBound ob;
		ob.name = new char[256];
		int j = 0;
		for (i += 1; str[i] && str[i] != '\n'; i++)
			ob.name[j++] = str[i];
		ob.name[j] = 0;
#ifdef DEBUG
		printf("new object : %s\n", ob.name);
#endif
		ob.p = nVertex;
		objectBounds.push_back(ob);
	}
	//group
	else if (_strcmpi(cmd, "g") == 0)
	{
		GroupBound gb;
		gb.name = new char[256];
		int j = 0;
		for (i += 1; str[i] && str[i] != '\n'; i++)
			gb.name[j++] = str[i];
		gb.name[j] = 0;

		gb.p = nVertex;
		groupBounds.push_back(gb);
	}
	//use material
	else if (_strcmpi(cmd, "usemtl") == 0)
	{
		char matNameStr[256];
		int j = 0;
		for (i += 1; str[i] && str[i] != '\n'; i++)
			matNameStr[j++] = str[i];
		matNameStr[j] = 0;
		currentMat = findMaterial(matNameStr);
#ifdef DEBUG
		if(currentMat!=NULL) printf("switch to material %s\n", currentMat->name);
#endif
	}
	//new lib
	else if (_strcmpi(cmd, "mtllib") == 0)
	{
		char buf[256];
		int j = 0;
		for (i += 1; str[i] && str[i] != '\n'; i++)
			buf[j++] = str[i];
		buf[j] = 0;
#ifdef DEBUG
		printf("mtllib %s\n", buf);
#endif
		readMTL(buf);
	}
	else
	{
		// unknown command, may be comments;
	}

}

Material* OBJModel::findMaterial(const char *name) const
{
	Material *mat;
	std::list<Material*>::const_iterator i;
	for (i = matList.begin(); i != matList.end(); i++)
	{
		if (_strcmpi((*i)->name, name) == 0)
		{
			mat = *i;
			return mat;
		}
	}
	return NULL;
}

void OBJModel::Draw(int mode)
{
	
	for (std::list<Triangle>::const_iterator it = triangleList.begin(); it != triangleList.end(); it++)
	{
		if (it->mat != NULL)
		{
			float Ka[4] = { it->mat->Ka[0], it->mat->Ka[1], it->mat->Ka[2], it->mat->Tr };
			float Ks[4] = { it->mat->Ks[0], it->mat->Ks[1], it->mat->Ks[2], it->mat->Tr };
			float Kd[4] = { it->mat->Kd[0], it->mat->Kd[1], it->mat->Kd[2], it->mat->Tr };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
			if (it->mat->t_Kd != NULL)
			{
				glBindTexture(GL_TEXTURE_2D, it->mat->t_Kd->id);
			}
		}
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < 3; i++)
		{
			if (it->mat != NULL && it->mat->t_Kd != NULL)
			{
				glTexCoord2d(uvArray[(*it).UV_Pnt[i] - 1].u, uvArray[(*it).UV_Pnt[i] - 1].v);
			}
			glVertex3d(vertexArray[(*it).Vertex[i] - 1].x, vertexArray[(*it).Vertex[i] - 1].y, vertexArray[(*it).Vertex[i] - 1].z);
		}
		glEnd();
	}
	
}

void OBJModel::readMTL(const char *filename)
{
	mat = NULL;
	FILE *nfp;
	fopen_s(&nfp, filename, "r");

	char buf[1024];
	while (fgets(buf, 1024, nfp))
	{
		readMTLString(buf);
	}

	if (mat != NULL)
	{
#ifdef DEBUG
		printf("new material added %s\n", mat->name);
#endif
		matList.push_back(mat);
	}
	mat = NULL;

	fclose(nfp);
}

void OBJModel::readMTLString(const char *str)
{
	char cmd[256];
	int i;
	for (i = 0; str[i] && str[i] != ' ' && i < 10; i++)
		cmd[i] = str[i];
	if (str[i] && i<8)cmd[i] = 0;
	else return;

	// new material
	if (_strcmpi(cmd, "newmtl") == 0)
	{
		if (mat != NULL)
		{
#ifdef DEBUG
			printf("new material added %s\n", mat->name);
#endif
			matList.push_back(mat);
		}
		mat = new Material();

		int j = 0;
		for (i += 1; str[i] && str[i] != '\n' && str[i] != ' '; i++)
			mat->name[j++] = str[i];
		mat->name[j] = 0;
		mat->Tr = 0;
		mat->t_bump = mat->t_d = mat->t_Ka = mat->t_Kd = mat->t_Ks = mat->t_Ns = NULL;
	}
	else if (_strcmpi(cmd, "Ns") == 0)
	{
		sscanf_s(str + i + 1, "%lf", &mat->Ns);
	}
	else if (_strcmpi(cmd, "Ka") == 0)
	{
		sscanf_s(str + i + 1, "%lf%lf%lf", &mat->Ka[0], &mat->Ka[1], &mat->Ka[2]);
	}
	else if (_strcmpi(cmd, "Kd") == 0)
	{
		sscanf_s(str + i + 1, "%lf%lf%lf", &mat->Kd[0], &mat->Kd[1], &mat->Kd[2]);
	}
	else if (_strcmpi(cmd, "Ks") == 0)
	{
		sscanf_s(str + i + 1, "%lf%lf%lf", &mat->Ks[0], &mat->Ks[1], &mat->Ks[2]);
	}
	else if (_strcmpi(cmd, "d") == 0)
	{
		sscanf_s(str + i + 1, "%lf", &mat->d);
	}
	else if (_strcmpi(cmd, "illum") == 0)
	{
		sscanf_s(str + i + 1, "%d", &mat->illum);
	}
	else if (_strcmpi(cmd, "map_Kd") == 0)
	{
		char buf[256];
		int j = 0;
		for (i += 1; str[i] && str[i] != '\n' && str[i] != ' '; i++)
			buf[j++] = str[i];
		buf[j] = 0;

		mat->t_Kd = new Texture(buf);
	}
	else
	{
	}

	//map_Ks, Ka ...

}

#endif	//OBJMODEL_H