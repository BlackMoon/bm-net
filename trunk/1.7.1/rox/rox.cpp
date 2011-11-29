// rox.cpp 
#pragma comment(lib, "../ext/release/ext")
#include "../dx/obj.h"
#include "../ext/ext.h"

#define WIN32_LEAN_AND_MEAN
#define STOCK_CONST			4 

struct mperiod
{
	char mon[4];
	float pmin, pmax, smin, smax;
	ULONG destp, dests;	
	USHORT date, month, year;	
	inline void fillDate()
	{			
		if (strcmp(mon, "Jan") == 0) month = 1;
		else if (strcmp(mon, "Feb") == 0) month = 2;
		else if (strcmp(mon, "Mar") == 0) month = 3;
		else if (strcmp(mon, "Apr") == 0) month = 4;
		else if (strcmp(mon, "May") == 0) month = 5;
		else if (strcmp(mon, "Jun") == 0) month = 6;
		else if (strcmp(mon, "Jul") == 0) month = 7;
		else if (strcmp(mon, "Aug") == 0) month = 8;
		else if (strcmp(mon, "Sep") == 0) month = 9;
		else if (strcmp(mon, "Oct") == 0) month = 10;
		else if (strcmp(mon, "Nov") == 0) month = 11;
		else if (strcmp(mon, "Dec") == 0) month = 12;
	}	
};
struct param
{
	char filename[MAX_PATH], shortname[16];
	float min, max;
	inline param()
	{
		min = 0.0f;
		max = 1.0f;		
	}
	inline void makeShort()
	{
		memset(shortname, 0, 16);
		getName(shortname, filename);
	}
};
struct wline
{
	UINT x, y;
	float z0, z1;
};				
struct dens
{
	float oil, watr, gas, kper;	
};
typedef vector<float> floatvector;
typedef vector<floatvector> floatmatrix;

bool dporo = 0;
char din[MAX_PATH], dout[MAX_PATH], symb;
float zmin, zmax;
SPACE space;

bool paramConvert(param* ppm)
{
	printf("[%s]\n", ppm->shortname);	
	
	char header[16],
		 file[MAX_PATH];	
	memset(file, 0, MAX_PATH);	

	strcpy(file, din);
	strcat(file, ppm->shortname);			
	
	FILE *istream = fopen(file, "r"),
		 *ostream;	
	if (!istream) return 0;	

	float fvalue0;
	floatvector fvector;
	floatmatrix fmatrix;
	int i, j, k; 	

	fscanf(istream, "%s\n", header);
	if (strcmp(header, "COOR") == 0)
	{			
		// read xmin
		UINT x, y;
		// first line
		fscanf(istream, "%u %u %f", &space.xMin, &space.yMax, &fvalue0); 
		zmin = zmax = -fvalue0;		
		fscanf(istream, "%u %u %f", &x, &y, &fvalue0); 		
		
		if (-fvalue0 < zmin) zmin = -fvalue0;
		if (-fvalue0 > zmax) zmax = -fvalue0;	

		for (i = 1; i < space.square() - 1; i++)
		{
			fscanf(istream, "%u %u %f", &x, &y, &fvalue0); 
			if (-fvalue0 < zmin) zmin = -fvalue0;
			if (-fvalue0 > zmax) zmax = -fvalue0;	

			fscanf(istream, "%u %u %f", &x, &y, &fvalue0); 
			if (-fvalue0 < zmin) zmin = -fvalue0;
			if (-fvalue0 > zmax) zmax = -fvalue0;	
		}
		// last line
		fscanf(istream, "%u %u %f", &space.xMax, &space.yMin, &fvalue0); 
		space.xStep = space.xLen() / space.nX + 1;
		space.yStep = space.yLen() / space.nY + 1;

		if (-fvalue0 < zmin) zmin = -fvalue0;
		if (-fvalue0 > zmax) zmax = -fvalue0;	

		fscanf(istream, "%u %u %f", &x, &y, &fvalue0); 
		if (-fvalue0 < zmin) zmin = -fvalue0;
		if (-fvalue0 > zmax) zmax = -fvalue0;			

		printf("\tSTEP %u %u\n", space.xStep, space.yStep);	
		printf("\tX %u %u\n", space.xMin, space.xMax);
		printf("\tY %u %u\n", space.yMin, space.yMax);		
		printf("\tZ %.2f %.2f\n", zmin, zmax);	
		
		strcpy(file, dout);
		strcat(file, "grid.txt");			

		ostream = fopen(file, "w");				
		if (!ostream) return 0;	
		fprintf(ostream, "SIZE %u %u %u\n", space.nX, space.nY, space.nZ);	
		fprintf(ostream, "STEP %u %u\n", space.xStep, space.yStep);	
		fprintf(ostream, "X %u %u\n", space.xMin, space.xMax);
		fprintf(ostream, "Y %u %u\n", space.yMin, space.yMax);		
		fprintf(ostream, "Z %.2f %.2f\n", zmin, zmax);			
		fclose(ostream);
	}
	else if (strcmp(header, "ZCORN") == 0)
	{			
		strcpy(file, dout);
		strcat(file, ppm->shortname);			

		ostream = fopen(file, "w");				
		if (!ostream) return 0;				
		fprintf(ostream, "%.3f %.3f\n", zmin, zmax);		
		
		float fvalue1;		
		for (i = 0; i < space.nZ; i++)
		{	
			printf("\tlayer%u", i + 1);
			// roof
			for (j = 0; j < ((space.nY - 1) << 1); j++)
			{					
				for (k = 0; k < space.nX - 1; k++)
				{
					fscanf(istream, "%f %f", &fvalue0, &fvalue1);					
					fvector.push_back(-fvalue0);					
				}
				fvector.push_back(-fvalue1);		// last value								
				fmatrix.push_back(fvector);
				fvector.clear();
			}			
			for (--j; j > 0; j -= 2)		
			{				
				fvector = fmatrix[j];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					fprintf(ostream, "%.3f ", fvalue0);									
				}
				fprintf(ostream, "\n");				
			}
			// last row
			fvector = fmatrix[0];
			for (k = 0; k < space.nX; k++)				
			{
				fvalue0 = fvector[k];
				fprintf(ostream, "%.3f ", fvalue0);																
			}
			fprintf(ostream, "\n");
			fmatrix.clear();
			fvector.clear();
			// sole
			for (j = 0; j < ((space.nY - 1) << 1); j++)
			{					
				for (k = 0; k < space.nX - 1; k++)
				{
					fscanf(istream, "%f %f", &fvalue0, &fvalue1);
					fvector.push_back(-fvalue0);					
				}
				fvector.push_back(-fvalue1);		// last value								
				fmatrix.push_back(fvector);
				fvector.clear();
			}			
			for (--j; j > 0; j -= 2)		
			{				
				fvector = fmatrix[j];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				
			}
			// last row
			fvector = fmatrix[0];
			for (k = 0; k < space.nX; k++)				
			{
				fvalue0 = fvector[k];
				fprintf(ostream, "%.3f ", fvalue0);																
			}
			fprintf(ostream, "\n");
			fmatrix.clear();
			fvector.clear();

			printf("\t\tok\n");			
		}
		fclose(ostream);
	}
	else if (strcmp(header, "K_X") == 0)
	{		
		if (symb != 'a')
		{
			printf("convert X_Permeability (y/n/a)?");					
			cin >> symb;
		}		
		if ((symb == 0x61) || (symb == 0x79))
		{
			strcpy(file, dout);
			strcat(file, ppm->shortname);
		
			ostream = fopen(file, "w");				
			if (!ostream) return 0;				
			fprintf(ostream, "%.3f %.3f\n", ppm->min, ppm->max);

			fscanf(istream, "VARI\n");
			for (i = 0; i < space.nZ; i++)
			{
				printf("\tlayer%u", i + 1);			
				for (j = 0; j < space.nY - 1; j++)
				{					
					for (k = 0; k < space.nX - 1; k++)
					{
						fscanf(istream, "%f", &fvalue0);
						fvector.push_back(fvalue0);					
					}
					fvector.push_back(fvalue0);		// last value								
					fmatrix.push_back(fvector);
					fvector.clear();
				}			
				for (--j; j >= 0; j--)		
				{				
					fvector = fmatrix[j];
					for (k = 0; k < space.nX; k++)				
					{
						fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

						fprintf(ostream, "%.3f ", fvalue0);																	
					}
					fprintf(ostream, "\n");				
				}
				// last row
				fvector = fmatrix[0];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				

				fmatrix.clear();
				fvector.clear();
				printf("\t\tok\n");			
			}			
			fclose(ostream);
		}
	}
	else if (strcmp(header, "K_Z") == 0)
	{		
		if (symb != 'a')
		{
			printf("convert Z_Permeability (y/n/a)?");					
			cin >> symb;
		}		
		if ((symb == 0x61) || (symb == 0x79))
		{
			strcpy(file, dout);
			strcat(file, ppm->shortname);
		
			ostream = fopen(file, "w");				
			if (!ostream) return 0;				
			fprintf(ostream, "%.3f %.3f\n", ppm->min, ppm->max);

			fscanf(istream, "VARI\n");
			for (i = 0; i < space.nZ; i++)
			{
				printf("\tlayer%u", i + 1);			
				for (j = 0; j < space.nY - 1; j++)
				{					
					for (k = 0; k < space.nX - 1; k++)
					{
						fscanf(istream, "%f", &fvalue0);
						fvector.push_back(fvalue0);					
					}
					fvector.push_back(fvalue0);		// last value								
					fmatrix.push_back(fvector);
					fvector.clear();
				}			
				for (--j; j >= 0; j--)		
				{				
					fvector = fmatrix[j];
					for (k = 0; k < space.nX; k++)				
					{
						fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

						fprintf(ostream, "%.3f ", fvalue0);																	
					}
					fprintf(ostream, "\n");				
				}
				// last row
				fvector = fmatrix[0];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				

				fmatrix.clear();
				fvector.clear();
				printf("\t\tok\n");			
			}			
			fclose(ostream);
		}
	}
	else if (strcmp(header, "PORO") == 0)
	{		
		if (symb != 'a')
		{
			printf("convert Porosity (y/n/a)?");					
			cin >> symb;
		}		
		if ((symb == 0x61) || (symb == 0x79))
		{
			strcpy(file, dout);
			strcat(file, ppm->shortname);			

			ostream = fopen(file, "w");						
			if (!ostream) return 0;				
			fprintf(ostream, "%.3f %.3f\n", ppm->min, ppm->max);
			
			fscanf(istream,"VARI\n");
			for (i = 0; i < space.nZ; i++)
			{
				printf("\tlayer%u", i + 1);			
				for (j = 0; j < space.nY - 1; j++)
				{					
					for (k = 0; k < space.nX - 1; k++)
					{
						fscanf(istream, "%f", &fvalue0);
						fvector.push_back(fvalue0);					
					}
					fvector.push_back(fvalue0);		// last value								
					fmatrix.push_back(fvector);
					fvector.clear();
				}			
				for (--j; j >= 0; j--)		
				{				
					fvector = fmatrix[j];
					for (k = 0; k < space.nX; k++)				
					{
						fvalue0 = fvector[k];
						if (fvalue0 < ppm->min) fvalue0 = ppm->min;
						if (fvalue0 > ppm->max) fvalue0 = ppm->max;	

						fprintf(ostream, "%.3f ", fvalue0);																	
					}
					fprintf(ostream, "\n");				
				}
				// last row
				fvector = fmatrix[0];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				

				fmatrix.clear();
				fvector.clear();
				printf("\t\tok\n");			
			}			
			fclose(ostream);
		}
	}
	else if (strcmp(header, "NTOG") == 0)
	{
		if (symb != 'a')
		{
			printf("convert NTG (y/n/a)?");					
			cin >> symb;
		}		
		if ((symb == 0x61) || (symb == 0x79))		
		{
			strcpy(file, dout);
			strcat(file, ppm->shortname);			

			ostream = fopen(file, "w");						
			if (!ostream) return 0;				
			fprintf(ostream, "%.3f %.3f\n", ppm->min, ppm->max);

			fscanf(istream, "VARI\n");			
			for (i = 0; i < space.nZ; i++)
			{
				printf("\tlayer%u", i + 1);			
				for (j = 0; j < space.nY - 1; j++)
				{					
					for (k = 0; k < space.nX - 1; k++)
					{
						fscanf(istream, "%f", &fvalue0);
						fvector.push_back(fvalue0);					
					}
					fvector.push_back(fvalue0);		// last value								
					fmatrix.push_back(fvector);
					fvector.clear();
				}			
				for (--j; j >= 0; j--)		
				{				
					fvector = fmatrix[j];
					for (k = 0; k < space.nX; k++)				
					{
						fvalue0 = fvector[k];
						if (fvalue0 < ppm->min) fvalue0 = ppm->min;
						if (fvalue0 > ppm->max) fvalue0 = ppm->max;

						fprintf(ostream, "%.3f ", fvalue0);																	
					}
					fprintf(ostream, "\n");				
				}
				// last row
				fvector = fmatrix[0];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) fvalue0 = ppm->min;
					if (fvalue0 > ppm->max) fvalue0 = ppm->max;

					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				

				fmatrix.clear();
				fvector.clear();
				printf("\t\tok\n");			
			}
			fclose(ostream);			
		}
	}
	else if (strcmp(header, "SWAT") == 0)
	{
		if (symb != 'a')
		{
			printf("convert SWAT (y/n/a)?");					
			cin >> symb;
		}		
		if ((symb == 0x61) || (symb == 0x79))		
		{
			strcpy(file, dout);
			strcat(file, ppm->shortname);			

			ostream = fopen(file, "w");						
			if (!ostream) return 0;							
			fprintf(ostream, "            \n");				// place for min & max			
			fscanf(istream, "VARI\n");			
			for (i = 0; i < space.nZ; i++)
			{
				printf("\tlayer%u", i + 1);			
				for (j = 0; j < space.nY - 1; j++)
				{					
					for (k = 0; k < space.nX - 1; k++)
					{
						fscanf(istream, "%f", &fvalue0);
						fvector.push_back(1 - fvalue0);					
					}
					fvector.push_back(1 - fvalue0);		// last value								
					fmatrix.push_back(fvector);
					fvector.clear();
				}			
				for (--j; j >= 0; j--)		
				{				
					fvector = fmatrix[j];
					for (k = 0; k < space.nX; k++)				
					{
						fvalue0 = fvector[k];
						if (fvalue0 < ppm->min) ppm->min = fvalue0;
						if (fvalue0 > ppm->max) ppm->max = fvalue0;

						fprintf(ostream, "%.3f ", fvalue0);																	
					}
					fprintf(ostream, "\n");				
				}
				// last row
				fvector = fmatrix[0];
				for (k = 0; k < space.nX; k++)				
				{
					fvalue0 = fvector[k];
					if (fvalue0 < ppm->min) ppm->min = fvalue0;
					if (fvalue0 > ppm->max) ppm->max = fvalue0;

					fprintf(ostream, "%.3f ", fvalue0);																	
				}
				fprintf(ostream, "\n");				

				fmatrix.clear();
				fvector.clear();
				printf("\t\tok\n");			
			}
			fseek(ostream, 0, SEEK_SET);
			fprintf(ostream, "%.3f %.3f", ppm->min, ppm->max);
			fclose(ostream);
		}
	}	
	fclose(istream);	

	printf("\n");		
	return 1;
}
bool outConvert(const char* filename, const HANDLE hfile)
{		
	bool bres = 1;
	HANDLE hmap = CreateFileMapping(hfile, 0, PAGE_READONLY, 0, 0, "fileMapping");      		
	LONG rest, size = GetFileSize(hfile, 0);
	void* pbase = MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0);		

	char buf[USHRT_MAX];
	char* pdest;		
	float fvalue, pmin, pmax, smin, smax;
	mperiod mp;
	vector<mperiod> mperiods0;
	int i = 0;
	
	for (i; i < size / USHRT_MAX; i++)
	{
		memset(buf, 0, USHRT_MAX);	
		memcpy(buf, (LPTSTR)pbase + i * USHRT_MAX, USHRT_MAX);		
		
		if (pdest = strstr(buf, "Map of PRES"))		
		{				
			memset(&mp, 0, 36);			
			mp.destp = (long)(pdest - buf) + i * USHRT_MAX;								
			
			if (sscanf(pdest, "Map of PRES at %u %s %u\n", &mp.date, mp.mon, &mp.year) == 0)			
				sscanf(pdest, "Map of PRESSURE at %u %s %u\n", &mp.date, mp.mon, &mp.year);
										
			mp.fillDate();			
			
			pdest -= 375;
			sscanf(pdest, "Pressure %f %f %f barsa\n", &mp.pmin, &mp.pmax, &fvalue);  
			pdest += 51;
			sscanf(pdest, "Soil %f %f %f frac\n", &mp.smin, &mp.smax, &fvalue);  			
		}		
		if (pdest = strstr(buf, "Map of soil"))		
		{				
			mp.dests = (long)(pdest - buf) + i * USHRT_MAX;										
			mperiods0.push_back(mp);			
		}		
	}	
	rest = size - i * USHRT_MAX;
	memset(buf, 0, USHRT_MAX);
	memcpy(buf, (LPTSTR)pbase + i * USHRT_MAX, rest);		
	
	if (!(pdest = strstr(buf, "Run finished successfully"))) 
		mperiods0.erase(&mperiods0.back());

	UnmapViewOfFile(pbase);
	CloseHandle(hmap);	
	CloseHandle(hfile);
	
	size = (long)mperiods0.size();
	if (size != 0)
	{
		mp = mperiods0[0];
		pmin = mp.pmin;
		pmax = mp.pmax;
		smin = mp.smin;
		smax = mp.smax;
		printf("\t[1]\t %u %s %u\n", mp.date, mp.mon, mp.year);

		for (i = 1; i < size; i++)
		{
			mp = mperiods0[i];
			if (pmin > mp.pmin) pmin = mp.pmin;
			if (pmax < mp.pmax) pmax = mp.pmax;
			if (smin > mp.smin) smin = mp.smin;
			if (smax < mp.smax) smax = mp.smax;

			printf("\t[%u]\t %u %s %u\n", i + 1, mp.date, mp.mon, mp.year);		
		}	
	
		int index = 0;
		vector<int> indices;		
		printf("\nchoose dates (0 - end):\n");		
		do
		{
			printf("\t");
			scanf("%d", &index);			
			if ((index < 0) || (index > (int)mperiods0.size())) 
			{
				printf("error\n");
				continue; 
			}
			indices.push_back(index);
		}
		while (index != 0);	
		indices.erase(&indices.back());
		// sort
		sort(indices.begin(), indices.end());		
		vector<mperiod> mperiods1;
		if (indices.size() == 0) mperiods1.swap(mperiods0);
		else
		{
			index = indices[0];
			mp = mperiods0[--index];
			mperiods1.push_back(mp);

			pmin = mp.pmin;
			pmax = mp.pmax;
			smin = mp.smin;
			smax = mp.smax;			

			for (i = 1; i < (int)indices.size(); i++)
			{
				index = indices[i];
				mp = mperiods0[--index];
				mperiods1.push_back(mp);

				if (pmin > mp.pmin) pmin = mp.pmin;
				if (pmax < mp.pmax) pmax = mp.pmax;
				if (smin > mp.smin) smin = mp.smin;
				if (smax < mp.smax) smax = mp.smax;			
			}	
		}
		indices.clear();	
		mperiods0.clear();	
		
		char* pfile = new char[MAX_PATH];	
		FILE *istream = fopen(filename, "r"), 
			 *pstream, *rstream, *sstream;							// actn;			 		
		
		// input stream
		if (!istream) return 0;

		memset(pfile, 0, MAX_PATH);		
		strcpy(pfile, dout);
		strcat(pfile, "p.txt");	
		// pres stream
		pstream = fopen(pfile, "w");
		if (!pstream) return 0;
		fprintf(pstream, "%.3f %.3f\n", pmin, pmax);
		
		memset(pfile, 0, MAX_PATH);		
		strcpy(pfile, dout);
		strcat(pfile, "actn.bin");	
		// actn stream
		size = (space.nX - 1) * (space.nY - 1) * space.nZ;
		bool* pbarr = new bool[size];						// active cells
		memset(pbarr, 0, size);

		rstream = fopen(pfile, "wb");
		if (!rstream) return 0;	
		
		memset(pfile, 0, MAX_PATH);		
		strcpy(pfile, dout);
		strcat(pfile, "soil.txt");	
		// soil stream
		sstream = fopen(pfile, "w");
		if (!sstream) return 0;	
		fprintf(sstream, "%.3f %.3f\n", smin, smax);			
		
		floatvector fvector;
		floatmatrix fmatrix;
		int ix, jy, kz, n, mpsize, value;		
			
		mp = mperiods1[0];						// first reading inactive cells					
		i = n = 0;						
		mpsize = (int)mperiods1.size();
		
		if (dporo)								// double poro
		{
			FILE *sstreamD;				

			memset(pfile, 0, MAX_PATH);		
			strcpy(pfile, dout);
			strcat(pfile, "soild.txt");
			// dporo soil stream	
			sstreamD = fopen(pfile, "w");
			delete pfile;
			if (!sstreamD) return 0;
			fprintf(sstreamD, "%.3f %.3f\n", smin, smax);

			// Map of PRESSURE
			printf("\nMap of PRESSURE at %u %s %u:\n", mp.date, mp.mon, mp.year);			
			fprintf(pstream, "%.3f %.3f\n", mp.pmin, mp.pmax);
			fseek(istream, mp.destp + 138, SEEK_SET);
			for (kz = 0; kz < space.nZ; kz++)
			{				
				printf("\tlayer%u", kz + 1);
				fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
				// read data
				if (fscanf(istream, "All values are %f", &fvalue) > 0)
				{
					for (jy = 0; jy < space.nY - 1; jy++)
					{					
						for (ix = 0; ix < space.nX - 1; ix++)
						{						
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				else
				{
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);			
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);					
							fvector.push_back(fvalue);
						}				
						// last value				
						fvector.push_back(fvalue);					
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				// save data			
				for (--jy; jy >= 0; jy--)		
				{				
					fvector = fmatrix[jy];
					for (ix = 0; ix < space.nX - 1; ix++)				
					{
						fvalue = fvector[ix];
						// inactive cells					
						if (fvalue >= mp.pmin - 1e-2) 
						{
							pbarr[i] = 1;
							n++;
						}
						i++;					
						fprintf(pstream, "%.3f\t", fvalue);																	
					}				
					fprintf(pstream, "%.3f\t", fvalue);																	
					fprintf(pstream, "\n");							
				}
				// last row
				fvector = fmatrix[0];
				for (ix = 0; ix < space.nX; ix++)				
				{
					fvalue = fvector[ix];				
					fprintf(pstream, "%.3f\t", fvalue);																
				}
				fprintf(pstream, "\n");
				fmatrix.clear();
				fvector.clear();
				
				printf("\t\tok\n");
			}
			fwrite(pbarr, 1, size, rstream);
			SAFE_DELETE_ARRAY(pbarr);
			fclose(rstream);			
			// Map of soil
			printf("\nMap of soil at %u %s %u:\n", mp.date, mp.mon, mp.year);	
			fprintf(sstream, "%.3f %.3f\n", mp.smin, mp.smax);
			fseek(istream, mp.dests + 134, SEEK_SET);				
			for (kz = 0; kz < space.nZ; kz++)
			{				
				fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
				// read data
				if (fscanf(istream, "All values are %f", &fvalue) > 0)
				{
					for (jy = 0; jy < space.nY - 1; jy++)
					{					
						for (ix = 0; ix < space.nX - 1; ix++)
						{						
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				else
				{
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}	
						// last value				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}			
				}
				// save data			
				for (--jy; jy >= 0; jy--)		
				{				
					fvector = fmatrix[jy];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];
						fprintf(sstream, "%.3f\t", fvalue);																	
					}
					fprintf(sstream, "\n");							
				}
				// last row
				fvector = fmatrix[0];
				for (ix = 0; ix < space.nX; ix++)				
				{
					fvalue = fvector[ix];				
					fprintf(sstream, "%.3f\t", fvalue);																
				}	
				fprintf(sstream, "\n");
				fmatrix.clear();
				fvector.clear();				
			}
			// dporo soil layers
			fprintf(sstreamD, "%.3f %.3f\n", mp.smin, mp.smax);
			for (kz = 0; kz < space.nZ; kz++)
			{						
				printf("\tlayer%u", kz + 1);
				fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
				// read data
				if (fscanf(istream, "All values are %f", &fvalue) > 0)
				{
					for (jy = 0; jy < space.nY - 1; jy++)
					{					
						for (ix = 0; ix < space.nX - 1; ix++)
						{						
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				else
				{
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}	
						// last value				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}			
				}
				// save data			
				for (--jy; jy >= 0; jy--)		
				{				
					fvector = fmatrix[jy];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];
						fprintf(sstreamD, "%.3f\t", fvalue);																	
					}
					fprintf(sstreamD, "\n");							
				}
				// last row
				fvector = fmatrix[0];
				for (ix = 0; ix < space.nX; ix++)				
				{
					fvalue = fvector[ix];				
					fprintf(sstreamD, "%.3f\t", fvalue);																
				}	
				fprintf(sstreamD, "\n");
				fmatrix.clear();
				fvector.clear();
		
				printf("\t\tok\n");
			}
			// second period 
			for (i = 1; i < mpsize; i++)
			{
				mp = mperiods1[i];				
				// Map of PRESSURE
				printf("\nMap of PRESSURE at %u %s %u:\n", mp.date, mp.mon, mp.year);	
				fprintf(pstream, "%.3f %.3f\n", mp.pmin, mp.pmax);				
				fseek(istream, mp.destp + 138, SEEK_SET);				
				for (kz = 0; kz < space.nZ; kz++)
				{					
					printf("\tlayer%u", kz + 1);
					fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
					// read data
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);			
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);						
							fvector.push_back(fvalue);
						}				
						// last value					
						fvector.push_back(fvalue);					
						fmatrix.push_back(fvector);
						fvector.clear();
					}
					// save data				
					for (--jy; jy >= 0; jy--)		
					{				
						fvector = fmatrix[jy];
						for (ix = 0; ix < space.nX; ix++)				
						{
							fvalue = fvector[ix];
							fprintf(pstream, "%.3f\t", fvalue);																	
						}
						fprintf(pstream, "\n");							
					}
					// last row
					fvector = fmatrix[0];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];				
						fprintf(pstream, "%.3f\t", fvalue);																
					}
					fprintf(pstream, "\n");
					fmatrix.clear();
					fvector.clear();					

					printf("\t\tok\n");
				}
				// Map of soil
				printf("\nMap of soil at %u %s %u:\n", mp.date, mp.mon, mp.year);	
				fprintf(sstream, "%.3f %.3f\n", mp.smin, mp.smax);				
				fseek(istream, mp.dests + 134, SEEK_SET);				
				for (kz = 0; kz < space.nZ; kz++)
				{					
					fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
					// read data
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);

					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
					// save data				
					for (--jy; jy >= 0; jy--)		
					{				
						fvector = fmatrix[jy];
						for (ix = 0; ix < space.nX; ix++)				
						{
							fvalue = fvector[ix];
							fprintf(sstream, "%.3f\t", fvalue);																	
						}
						fprintf(sstream, "\n");							
					}
					// last row
					fvector = fmatrix[0];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];				
						fprintf(sstream, "%.3f\t", fvalue);																
					}
					fprintf(sstream, "\n");
					fmatrix.clear();
					fvector.clear();					
				}
				// dporo soil layers
				fprintf(sstreamD, "%.3f %.3f\n", mp.smin, mp.smax);
				for (kz = 0; kz < space.nZ; kz++)
				{				
					printf("\tlayer%u", kz + 1);
					fscanf(istream, "\nLayer iz=%u\nix=   :", &value);	
					// read data
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);

					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
					// save data				
					for (--jy; jy >= 0; jy--)		
					{				
						fvector = fmatrix[jy];
						for (ix = 0; ix < space.nX; ix++)				
						{
							fvalue = fvector[ix];
							fprintf(sstreamD, "%.3f\t", fvalue);																	
						}
						fprintf(sstreamD, "\n");							
					}
					// last row
                    fvector = fmatrix[0];
                    for (ix = 0; ix < space.nX; ix++)               
                    {
                        fvalue = fvector[ix];               
                        fprintf(sstreamD, "%.3f\t", fvalue);                                                                
                    }
                    fprintf(sstreamD, "\n");
                    fmatrix.clear();
					fvector.clear();
					
					printf("\t\tok\n");
				}
			}
			fclose(sstreamD);
		}
		else
		{	// Map of PRESSURE
			printf("\nMap of PRESSURE at %u %s %u:\n", mp.date, mp.mon, mp.year);			
			fprintf(pstream, "%.3f %.3f\n", mp.pmin, mp.pmax);
			fseek(istream, mp.destp + 138, SEEK_SET);
			for (kz = 0; kz < space.nZ; kz++)
			{
				printf("\tlayer%u", kz + 1);
				fscanf(istream, "\nLayer iz=%u\nix=   :", &value);				
				// read data
				if (fscanf(istream, "All values are %f", &fvalue) > 0)
				{
					for (jy = 0; jy < space.nY - 1; jy++)
					{					
						for (ix = 0; ix < space.nX - 1; ix++)
						{						
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				else
				{
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);			
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);					
							fvector.push_back(fvalue);
						}				
						// last value				
						fvector.push_back(fvalue);					
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				// save data			
				for (--jy; jy >= 0; jy--)		
				{				
					fvector = fmatrix[jy];
					for (ix = 0; ix < space.nX - 1; ix++)				
					{
						fvalue = fvector[ix];
						// inactive cells					
						if (fvalue >= mp.pmin - 1e-2) 
						{
							pbarr[i] = 1;
							n++;
						}
						i++;					
						fprintf(pstream, "%.3f\t", fvalue);																	
					}				
					fprintf(pstream, "%.3f\t", fvalue);																	
					fprintf(pstream, "\n");							
				}
				// last row
				fvector = fmatrix[0];
				for (ix = 0; ix < space.nX; ix++)				
				{
					fvalue = fvector[ix];				
					fprintf(pstream, "%.3f\t", fvalue);																
				}
				fprintf(pstream, "\n");
				fmatrix.clear();
				fvector.clear();

				printf("\t\tok\n");
			}
			fwrite(pbarr, 1, size, rstream);
			delete [] pbarr;
			fclose(rstream);
			// Map of soil
			printf("\nMap of soil at %u %s %u:\n", mp.date, mp.mon, mp.year);	
			fprintf(sstream, "%.3f %.3f\n", mp.smin, mp.smax);
			fseek(istream, mp.dests + 134, SEEK_SET);				
			for (kz = 0; kz < space.nZ; kz++)
			{
				printf("\tlayer%u", kz + 1);
				fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
				//
				if (fscanf(istream, "All values are %f", &fvalue) > 0)
				{
					for (jy = 0; jy < space.nY - 1; jy++)
					{					
						for (ix = 0; ix < space.nX - 1; ix++)
						{						
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
				}
				else
				{
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}	
						// last value				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}			
				}
				// save data			
				for (--jy; jy >= 0; jy--)		
				{				
					fvector = fmatrix[jy];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];
						fprintf(sstream, "%.3f\t", fvalue);																	
					}
					fprintf(sstream, "\n");							
				}
				// last row
				fvector = fmatrix[0];
				for (ix = 0; ix < space.nX; ix++)				
				{
					fvalue = fvector[ix];				
					fprintf(sstream, "%.3f\t", fvalue);																
				}	
				fprintf(sstream, "\n");
				fmatrix.clear();
				fvector.clear();
		
				printf("\t\tok\n");
			}
			// second period 
			for (i = 1; i < mpsize; i++)
			{
				mp = mperiods1[i];				
				// Map of PRESSURE
				printf("\nMap of PRESSURE at %u %s %u:\n", mp.date, mp.mon, mp.year);	
				fprintf(pstream, "%.3f %.3f\n", mp.pmin, mp.pmax);
				fseek(istream, mp.destp + 138, SEEK_SET);				
				for (kz = 0; kz < space.nZ; kz++)
				{
					printf("\tlayer%u", kz + 1);
					fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
					// read data
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);			
					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);						
							fvector.push_back(fvalue);
						}				
						// last value					
						fvector.push_back(fvalue);					
						fmatrix.push_back(fvector);
						fvector.clear();
					}
					// save data				
					for (--jy; jy >= 0; jy--)		
					{				
						fvector = fmatrix[jy];
						for (ix = 0; ix < space.nX; ix++)				
						{
							fvalue = fvector[ix];
							fprintf(pstream, "%.3f\t", fvalue);																	
						}
						fprintf(pstream, "\n");							
					}
					// last row
					fvector = fmatrix[0];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];				
						fprintf(pstream, "%.3f\t", fvalue);																
					}
					fprintf(pstream, "\n");
					fmatrix.clear();
					fvector.clear();
					printf("\t\tok\n");
				}
				// Map of soil
				printf("\nMap of soil at %u %s %u:\n", mp.date, mp.mon, mp.year);	
				fprintf(sstream, "%.3f %.3f\n", mp.smin, mp.smax);
				fseek(istream, mp.dests + 134, SEEK_SET);				
				for (kz = 0; kz < space.nZ; kz++)
				{
					printf("\tlayer%u", kz + 1);
					fscanf(istream, "\nLayer iz=%u\nix=   :", &value);			
				
					for (ix = 0; ix < space.nX - 1; ix++)			
						fscanf(istream, "%u", &value);

					for (jy = 0; jy < space.nY - 1; jy++)
					{
						fscanf(istream, "\nRow iy=%u:", &value);
				
						for (ix = 0; ix < space.nX - 1; ix++)
						{
							fscanf(istream, "%f", &fvalue);
							fvector.push_back(fvalue);
						}				
						fvector.push_back(fvalue);
						fmatrix.push_back(fvector);
						fvector.clear();
					}
					// save data				
					for (--jy; jy >= 0; jy--)		
					{				
						fvector = fmatrix[jy];
						for (ix = 0; ix < space.nX; ix++)				
						{
							fvalue = fvector[ix];
							fprintf(sstream, "%.3f\t", fvalue);																	
						}
						fprintf(sstream, "\n");							
					}
					// last row
					fvector = fmatrix[0];
					for (ix = 0; ix < space.nX; ix++)				
					{
						fvalue = fvector[ix];				
						fprintf(sstream, "%.3f\t", fvalue);																
					}
					fprintf(sstream, "\n");
					fmatrix.clear();
					fvector.clear();
			
					printf("\t\tok\n");
				}
			}		
		}
		fclose(istream);
		fclose(pstream);
		fclose(sstream);			
		
		printf("\nnumber of active cells\t%u", n);		
		printf("\nnumber of periods\t%u\n\n", mpsize);		
	}
	return bres;
}
bool wellConvert(const char* filename)
{
	printf("[%s]\n", "trackwell.txt");	
	
	char name[9],
		 file[MAX_PATH];	
	memset(file, 0, MAX_PATH);		
	
	size_t len = strlen(filename);
	strcpy(file, din);
	strncat(file, filename, --len);
	
	FILE *istream = fopen(file, "r"),
		 *ostream;	
	
	if (!istream) return 0;		
	
	strcpy(file, dout);
	strcat(file, "\\well.txt");			
	
	ostream = fopen(file, "w");
	if (!ostream) return 0;
	
	bool bvalid;
	size_t i;
	wline _wline;
	vector <wline> wlines;
	
	while (fscanf(istream, "%s\n", name) == 1)
	{
		bvalid = 0;		
		while (fscanf(istream, "%u %u %f %f\n", &_wline.x, &_wline.y, &_wline.z0, &_wline.z1) == 4)
		{
			if (BOUNDS(space).ptBelong(_wline.x, _wline.y))		
			{
				bvalid = 1;
				wlines.push_back(_wline);
			}
		}
		fscanf(istream, "/\n");
		
		if (bvalid)
		{
			printf("\twell %s\n", name);		
			fprintf(ostream, "     %s\n", name);
			for (i = 0; i < wlines.size(); i++)
			{
				fprintf(ostream, "%u %u %.2f\n", wlines[i].x, wlines[i].y, -wlines[i].z0);				
			}
			wlines.clear();
			fprintf(ostream, "/\n");
		}		
	}
	fclose(ostream);	
	fclose(istream);	

	printf("\n");		
	return 1;
}
bool stockConvert(vector<param> *pvec, dens* pdens)
{
	printf("[%s]\n", "stock");			
	
	bool bres = 1,
		 bm = 0, bn = 0, bs = 0, bz = 0;				// can close streams	
	FILE *streamM, *streamN, *streamS, *streamZ;		

	try
	{		
		char file[MAX_PATH];					
		int i;
		param pm;	
		for (i = 0; i < STOCK_CONST; i++)
		{
			pm = pvec->at(i);
			memset(file, 0, MAX_PATH);	
			strcpy(file, dout);
			strcat(file, pm.shortname);		
			if (strcmp(pm.shortname, "M.txt") == 0)
			{
				streamM = fopen(file, "r");
				if (!streamM) throw 0;
				bm = 1;
			}
			if (strcmp(pm.shortname, "NTOG.txt") == 0)
			{
				streamN = fopen(file, "r");				
				if (!streamN) throw 0;
				bn = 1;
			}
			if (strcmp(pm.shortname, "SWAT.txt") == 0)
			{
				streamS = fopen(file, "r");		
				if (!streamS) throw 0;
				bs = 1;
			}
			if (strcmp(pm.shortname, "ZCORN.txt") == 0)
			{
				streamZ = fopen(file, "r");	
				if (!streamZ) throw 0;
				bz = 1;
			}
		}	
		memset(file, 0, MAX_PATH);	
		strcpy(file, dout);
		strcat(file, "stock.txt");		

		FILE *ostream = fopen(file, "w");				
		if (!ostream) throw 0;		
		fprintf(ostream, "            \n");				// place for min & max
	
		float fvalue, fmin = 10.0f, fmax = 0.0f,
			  fvalueM, fvalueN, fvalueS, fvalueZ;	

		floatvector fvector, fvectorh;
		floatmatrix fmatrix, fmatrixh;

		fscanf(streamM, "%f %f", &fvalueS, &fvalueS);
		fscanf(streamN, "%f %f", &fvalueN, &fvalueN);
		fscanf(streamS, "%f %f", &fvalueS, &fvalueS);
		fscanf(streamZ, "%f %f", &fvalueZ, &fvalueZ);	

		int j, k;
		for (i = 0; i < space.nZ; i++)
		{
			printf("\tlayer%u", i + 1);
			// m, ntog, s, roof
			for (j = 0; j < space.nY; j++)
			{
				for (k = 0; k < space.nX; k++)
				{
					fscanf(streamM, "%f", &fvalueM);
					fscanf(streamN, "%f", &fvalueN);
					fscanf(streamS, "%f", &fvalueS);
					fscanf(streamZ, "%f", &fvalueZ);
					
					fvalue = fvalueM * fvalueN * fvalueS; 						
				
					fvector.push_back(fvalue);
					fvectorh.push_back(fvalueZ);
				}
				fmatrix.push_back(fvector);
				fmatrixh.push_back(fvectorh);
			
				fvector.clear();
				fvectorh.clear();			
			}
			// sole
			for (j = 0; j < space.nY; j++)
			{			
				fvectorh = fmatrixh[j];
				for (k = 0; k < space.nX; k++)
				{
					fscanf(streamZ, "%f", &fvalueZ);								
					float f0 = fmatrix[j][k], f1 = fvectorh[k];

					fvalue = fmatrix[j][k] * (fvectorh[k] - fvalueZ);							 
					fmatrix[j][k] = fvalue;
				}	
				fvectorh.clear();
			}
			fmatrixh.clear();
			// save
			for (j = 0; j < space.nY; j++)
			{			
				fvector = fmatrix[j];
				for (k = 0; k < space.nX; k++)
				{
					fvalue = fvector[k] * pdens->kper * pdens->oil / 1000.0f;
					if (fvalue < fmin) fmin = fvalue;
					if (fvalue > fmax) fmax = fvalue;	
					fprintf(ostream, "%.3f ", fvalue);
				}			
				fprintf(ostream, "\n");	
				fvector.clear();
			}		
			fmatrix.clear();
			printf("\t\tok\n");		
		}
		fseek(ostream, 0, SEEK_SET);
		fprintf(ostream, "%.3f %.3f", fmin, fmax);		
		fclose(ostream);
	}
	catch (int)
	{		
		bres = 0;
	}
	// close streams
	if (bm) fclose(streamM);
	if (bn) fclose(streamN);
	if (bs) fclose(streamS);
	if (bz) fclose(streamZ);
	
	printf("\n");		
	return bres;
}
bool roxConvert(const char* filename)
{		
	char file[MAX_PATH];			
	WIN32_FIND_DATA find;

	memset(file, 0, MAX_PATH);	
	strcpy(file, din);
	strcat(file, "\\*.out");
	
	HANDLE hfile0 = FindFirstFile(file, &find);	
	if (hfile0 == INVALID_HANDLE_VALUE) 
	{
		printf("error: %s files not found\n", file);
		return 0;
	}
	FindClose(hfile0);
	
	memset(file, 0, MAX_PATH);
	strcpy(file, din);
	strcat(file, find.cFileName);	
	
	hfile0 = CreateFile(file, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);	
	char buf[USHRT_MAX];
	memset(buf, 0, USHRT_MAX);
	ULONG nbytes;	
	ReadFile(hfile0, buf, USHRT_MAX, &nbytes, 0);		
	// 
	HANDLE hfile1 = CreateFile(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);	
	if (hfile1 == INVALID_HANDLE_VALUE) 
	{		
		printf("error: %s opening failed\n", filename);
		return 0;
	}	
	LONG size = GetFileSize(hfile1, 0);	
	
	char* pbuf = new char[size];
	memset(pbuf, 0, size);	
	ReadFile(hfile1, pbuf, size, &nbytes, 0);
	CloseHandle(hfile1);
	// dim
	memset(&space, 0, sizeof(space));				
	char* pdest0 = 0;
	
	if (pdest0 = strstr(pbuf, "DPORO")) dporo = 1;	
	if (pdest0 = strstr(pbuf, "SIZE"))
	{
		sscanf(pdest0, "SIZE %u %u %u", &space.nX, &space.nY, &space.nZ);
		space.nX++;
		space.nY++;
		if (dporo) space.nZ /= 2;
		printf("SIZE %u %u %u\n\n", space.nX, space.nY, space.nZ);
	}
	if (!outConvert(file, hfile0)) return 0;	
	// density & kper
	dens _dens;
	if (pdest0 = strstr(pbuf, "FLUID BLACKOIL"))
	{	// water		
		if (pdest0 = strstr(pdest0, "WATR")) 
		{
			float watr;
			sscanf(pdest0, "WATR\n %f %f", &watr, &_dens.watr);
		}
		// oil
		if (pdest0 = strstr(pdest0, "BASIC")) sscanf(pdest0, "BASIC\n %f", &_dens.oil);		
		// gas, kper		
		if (pdest0 = strstr(pdest0, "OPVT"))
		{
			float p, visc; 
			sscanf(pdest0, "OPVT\n %f %f %f %f", &p, &_dens.kper, &visc, &_dens.gas);
		}
	}
	// params	
	if (pdest0 = strstr(pbuf, "include"))
	{
		char *pdest1 = 0;			
		char line[MAX_PATH];
		size_t len;		
		param pm;
		vector<param> params;

		do	
		{			
			memset(line, 0, MAX_PATH);
			memset(pm.filename, 0, MAX_PATH);
			sscanf(pdest0, "include '%s", line);
		
			len = strlen(line);				
			strncpy(pm.filename, line, --len);	
			pm.makeShort();
			
			if (strlen(pm.shortname) > 0)
			{
				if (strcmp(pm.shortname, "K.txt") == 0)
				{
					pdest1 = strstr(buf, "Permx"); 
					sscanf(pdest1, "Permx %f %f", &pm.min, &pm.max);						
				}
				else if (strcmp(pm.shortname, "KZ.txt") == 0)
				{
					pdest1 = strstr(buf, "Permz"); 
					sscanf(pdest1, "Permz %f %f", &pm.min, &pm.max);						
				}
				else if (strcmp(pm.shortname, "M.txt") == 0)
				{
					pdest1 = strstr(buf, "Porosity"); 
					sscanf(pdest1, "Porosity %f %f", &pm.min, &pm.max);	
					params.push_back(pm);
				}
				else if (strcmp(pm.shortname, "NTOG.txt") == 0)
				{
					pdest1 = strstr(buf, "NTG"); 
					sscanf(pdest1, "NTG %f %f", &pm.min, &pm.max);						
					params.push_back(pm);
				}
				else if (strcmp(pm.shortname, "SWAT.txt") == 0)
				{
					pm.min = 1.0f;
					pm.max = 0.0f;	
					params.push_back(pm);
				}
				else if (strcmp(pm.shortname, "ZCORN.txt") == 0)
				{					
					params.push_back(pm);
				}
				paramConvert(&pm);	
			}		
			pdest0++;			
			pdest0 = strstr(pdest0, "include");		
		}	
		while (pdest0);
		// stock
		if (params.size() == STOCK_CONST) stockConvert(&params, &_dens);
		params.clear();
	}	
	// wells
	if (pdest0 = strstr(pbuf, "TFIL"))
	{
		sscanf(pdest0, "TFIL\n'%s", file);
		wellConvert(file);	
	}		
	SAFE_DELETE_ARRAY(pbuf);		
	
	return 1;
}
int main(int argc, char* argv[])
{
	if (argc == 1) 
	{
		printf("usage: rox [filename]\n");
		return 0;
	}
	// head
	printf("\n");
	printf("\t\t\t--------------------\n");
	printf("\t\t\tRoxar Data Convertor\n");
	printf("\t\t\t--------------------\n");
	printf("\n");	

	char dat[MAX_PATH];	
	memset(dat, 0, MAX_PATH);

	memset(din, 0, MAX_PATH);		
	getDir(din, argv[1]); 
	if (strlen(din) == 0) 
	{
		getDir(din, argv[0]);
		strcpy(dat, din);
		strcat(dat, argv[1]);
	}
	else strcpy(dat, argv[1]);
	
	memset(dout, 0, MAX_PATH);	
	strcpy(dout, din);
	strcat(dout, "out");
	CreateDirectory(dout, 0);	
	strcat(dout, "\\");

	printf("%s\n\n", dat);
	if (!roxConvert(dat)) return 1;	
	
	printf("Press any key to continue\n");
	getch();
	return 0;
}