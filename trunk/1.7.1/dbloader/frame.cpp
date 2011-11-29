// frame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "dbloader.h"

#include "frame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// tables
#define HORZ				"horz"
#define PLAST				"plast"
#define	HORZ_P				"horz_params"
#define	PLAST_P				"pl_params"
#define	SLICE_P				"sl_params"
#define	CONT				"contours"
#define GRIDS				"grids"
#define WELL				"well"
// CMainFrame
bool createHorz(CDaoDatabase* pdb)
{	
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi;

	ifi.m_strName = "code";
	ifi.m_bDescending = 0;
	
	try
	{			
		td.Create(HORZ);
		// code
		fi.m_strName = "code";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbFixedField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// xmin
		fi.m_strName = "xmin";
		fi.m_nOrdinalPosition = 2;		
		td.CreateField(fi);
		// ymin
		fi.m_strName = "ymin";	
		fi.m_nOrdinalPosition = 3;		
		td.CreateField(fi);
		// nx
		fi.m_strName = "nx";
		fi.m_nType = dbInteger;
		fi.m_lSize = 0x2;	
		fi.m_nOrdinalPosition = 4;		
		td.CreateField(fi);
		// ny
		fi.m_strName = "ny";
		fi.m_nOrdinalPosition = 5;		
		td.CreateField(fi);
		// xstep
		fi.m_strName = "xstep";
		fi.m_nOrdinalPosition = 6;		
		td.CreateField(fi);
		// ystep
		fi.m_strName = "ystep";
		fi.m_nOrdinalPosition = 7;		
		td.CreateField(fi);
		// collmin
		fi.m_strName = "collmin";
		fi.m_nType = dbSingle;
		fi.m_lSize = 0x4;
		fi.m_strDefaultValue = "0";
		fi.m_nOrdinalPosition = 8;		
		td.CreateField(fi);
		// m
		fi.m_strName = "m";
		fi.m_strDefaultValue = "-1";
		fi.m_nOrdinalPosition = 9;		
		td.CreateField(fi);
		// s
		fi.m_strName = "s";
		fi.m_nOrdinalPosition = 10;		
		td.CreateField(fi);
		// k
		fi.m_strName = "k";
		fi.m_nOrdinalPosition = 11;		
		td.CreateField(fi);
		// cl
		fi.m_strName = "cl";
		fi.m_nOrdinalPosition = 12;		
		td.CreateField(fi);
		// geo_prop
		fi.m_strName = "geo_prop";
		fi.m_nType = dbMemo;
		fi.m_lSize = 0x0;
		fi.m_nOrdinalPosition = 13;		
		fi.m_strDefaultValue = "";
		fi.m_bRequired = 0;
		fi.m_bAllowZeroLength = 1;
		td.CreateField(fi);
		// primary key		
		ii.m_strName = "pk_horz";		
		ii.m_nFields = 1;
		ii.m_pFieldInfos = &ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);
		
		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createPlast(CDaoDatabase* pdb)
{
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[2];
	ifi[0].m_strName = "horz";	 
	ifi[1].m_strName = "code";
	ifi[0].m_bDescending = ifi[1].m_bDescending = 0;
	
	try
	{			
		td.Create(PLAST);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbFixedField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// code
		fi.m_strName = "code";
		fi.m_nType = dbByte;
		fi.m_lSize = 0x1;
		fi.m_nOrdinalPosition = 2;		
		fi.m_strDefaultValue = "0";
		td.CreateField(fi);
		// pl_order
		fi.m_strName = "pl_order";	
		fi.m_nOrdinalPosition = 3;		
		td.CreateField(fi);
		// cont_from_horz
		fi.m_strName = "cont_from_horz";
		fi.m_nType = dbBoolean;
		fi.m_nOrdinalPosition = 4;	
		fi.m_strDefaultValue = "0";
		td.CreateField(fi);
		// slice
		fi.m_strName = "slice";		
		fi.m_nType = dbMemo;
		fi.m_lSize = 0x0;
		fi.m_nOrdinalPosition = 5;		
		fi.m_strDefaultValue = "";
		fi.m_bRequired = 0;
		fi.m_bAllowZeroLength = 1;
		td.CreateField(fi);		
		// primary key		
		ii.m_strName = "pk_plast";		
		ii.m_nFields = 2;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;				
		td.CreateIndex(ii);		

		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createGrid(CDaoDatabase* pdb)
{
	bool bres = 1;
	CDaoTableDef td(pdb);
	
	CDaoFieldInfo fi;
	fi.m_lCollatingOrder = 0;

	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[4];
	
	ifi[0].m_strName = "horz";	 
	ifi[1].m_strName = "plast";
	ifi[2].m_strName = "slice";
	ifi[3].m_strName = "field";
	ifi[0].m_bDescending = ifi[1].m_bDescending = 
		ifi[2].m_bDescending = ifi[3].m_bDescending = 0;
	
	try
	{			
		td.Create(GRIDS);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbFixedField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// code
		fi.m_strName = "plast";
		fi.m_nType = dbByte;
		fi.m_lSize = 0x1;
		fi.m_nOrdinalPosition = 2;	
		fi.m_strDefaultValue = "0";
		td.CreateField(fi);
		// slice
		fi.m_strName = "slice";	
		fi.m_nOrdinalPosition = 3;			
		td.CreateField(fi);
		// grid_type
		fi.m_strName = "grid_type";				
		fi.m_nOrdinalPosition = 6;		
		td.CreateField(fi);	
		// count_type
		fi.m_strName = "count_type";						
		fi.m_nOrdinalPosition = 10;		
		fi.m_strDefaultValue = "3";
		td.CreateField(fi);	
		// src_type
		fi.m_strName = "src_type";								
		fi.m_nOrdinalPosition = 11;		
		fi.m_strDefaultValue = "0";		
		td.CreateField(fi);
		// valid
		fi.m_strName = "valid";						
		fi.m_nOrdinalPosition = 17;
		td.CreateField(fi);	
		// field
		fi.m_strName = "field";
		fi.m_nType = dbText;
		fi.m_lSize = 0x5;	
		fi.m_nOrdinalPosition = 4;	
		fi.m_strDefaultValue = "";
		fi.m_lAttributes = dbVariableField;			
		td.CreateField(fi);
		// grid_name
		fi.m_strName = "grid_name";				
		fi.m_lSize = 0x32;	
		fi.m_nOrdinalPosition = 5;		
		fi.m_strDefaultValue = "";
		fi.m_bRequired = 0;
		fi.m_bAllowZeroLength = 1;
		td.CreateField(fi);	
		// formula
		fi.m_strName = "formula";		
		fi.m_lSize = 0xff;	
		fi.m_nOrdinalPosition = 13;			
		td.CreateField(fi);	
		// pts
		fi.m_strName = "pts";				
		fi.m_nType = dbMemo;
		fi.m_lSize = 0x0;
		fi.m_nOrdinalPosition = 7;			
		fi.m_strDefaultValue = "";
		fi.m_bRequired = 0;
		fi.m_bAllowZeroLength = 1;
		td.CreateField(fi);		
		// add_pts
		fi.m_strName = "add_pts";						
		fi.m_nOrdinalPosition = 8;					
		td.CreateField(fi);		
		// del_pts
		fi.m_strName = "del_pts";						
		fi.m_nOrdinalPosition = 9;					
		td.CreateField(fi);			
		// src_sql
		fi.m_strName = "src_sql";		
		fi.m_nOrdinalPosition = 12;		
		td.CreateField(fi);	
		// grid_options
		fi.m_strName = "grid_options";						
		fi.m_nOrdinalPosition = 16;	
		td.CreateField(fi);	
		// srf_options
		fi.m_strName = "srf_options";								
		fi.m_nOrdinalPosition = 17;	
		td.CreateField(fi);			
		// grid		
		fi.m_strName = "grid";								
		fi.m_nType = dbLongBinary;
		fi.m_lSize = 0x0;
		fi.m_nOrdinalPosition = 14;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);		
		// date
		fi.m_strName = "date";								
		fi.m_nType = dbDate;
		fi.m_lSize = 0x8;
		fi.m_nOrdinalPosition = 15;	
		td.CreateField(fi);		
		// primary key		
		ii.m_strName = "pk_grids";		
		ii.m_nFields = 4;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);		

		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createCont(CDaoDatabase* pdb)
{
	bool bres = 1;
	CDaoTableDef td(pdb);
	
	CDaoFieldInfo fi;
	fi.m_lCollatingOrder = 0;

	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[4];
	
	ifi[0].m_strName = "horz";	 
	ifi[1].m_strName = "plast";
	ifi[2].m_strName = "type";
	ifi[3].m_strName = "num";
	ifi[0].m_bDescending = ifi[1].m_bDescending = 
		ifi[2].m_bDescending = ifi[3].m_bDescending = 0;
	
	try
	{			
		td.Create(CONT);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbFixedField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// plast
		fi.m_strName = "plast";
		fi.m_nType = dbByte;
		fi.m_lSize = 0x1;
		fi.m_nOrdinalPosition = 2;	
		fi.m_strDefaultValue = "0";
		td.CreateField(fi);
		// num
		fi.m_strName = "num";				
		fi.m_nOrdinalPosition = 4;		
		td.CreateField(fi);	
		// attr
		fi.m_strName = "attr";								
		fi.m_nOrdinalPosition = 7;					
		td.CreateField(fi);		
		// joined
		fi.m_strName = "joined";						
		fi.m_nType = dbBoolean;		
		fi.m_nOrdinalPosition = 6;					
		td.CreateField(fi);				
		// type
		fi.m_strName = "type";	
		fi.m_nType = dbText;
		fi.m_lSize = 0x14;
		fi.m_nOrdinalPosition = 3;			
		fi.m_lAttributes = dbVariableField;			
		fi.m_strDefaultValue = "";		
		fi.m_bAllowZeroLength = 1;
		td.CreateField(fi);	
		// pts
		fi.m_strName = "pts";				
		fi.m_nType = dbMemo;
		fi.m_lSize = 0x0;
		fi.m_nOrdinalPosition = 5;		
		td.CreateField(fi);	
		// primary key		
		ii.m_strName = "pk_cont";		
		ii.m_nFields = 4;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);		

		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createWell(CDaoDatabase* pdb)
{
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi;

	ifi.m_strName = "name";
	ifi.m_bDescending = 0;
	
	try
	{			
		td.Create(WELL);
		// name
		fi.m_strName = "name";
		fi.m_nType = dbText;
		fi.m_lSize = 0x7;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbVariableField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// site
		fi.m_strName = "site";
		fi.m_nType = dbByte;
		fi.m_lSize = 0x1;
		fi.m_nOrdinalPosition = 2;			
		fi.m_bRequired = 0;	
		fi.m_lAttributes = dbFixedField;
		td.CreateField(fi);		
		// cond_dmetr
		fi.m_strName = "cond_dmetr";	
		fi.m_nType = dbInteger;	
		fi.m_lSize = 0x2;
		fi.m_nOrdinalPosition = 16;				
		td.CreateField(fi);
		// expl_dmetr
		fi.m_strName = "expl_dmetr";			
		fi.m_nOrdinalPosition = 17;				
		td.CreateField(fi);
		// tech_dmetr
		fi.m_strName = "tech_dmetr";			
		fi.m_nOrdinalPosition = 18;				
		td.CreateField(fi);
		// xmouth
		fi.m_strName = "xmouth";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;	
		fi.m_nOrdinalPosition = 4;		
		td.CreateField(fi);
		// ymouth
		fi.m_strName = "ymouth";
		fi.m_nOrdinalPosition = 5;		
		td.CreateField(fi);
		// xfc
		fi.m_strName = "xfc";
		fi.m_nType = dbLong;				
		fi.m_nOrdinalPosition = 8;		
		td.CreateField(fi);
		// yfc
		fi.m_strName = "yfc";		
		fi.m_nOrdinalPosition = 9;		
		td.CreateField(fi);
		// alt
		fi.m_strName = "alt";
		fi.m_nType = dbSingle;		
		fi.m_nOrdinalPosition = 6;		
		td.CreateField(fi);
		// mgn_decl
		fi.m_strName = "mgn_decl";
		fi.m_nOrdinalPosition = 7;		
		td.CreateField(fi);		
		// cont_cmt_rise
		fi.m_strName = "cont_cmt_rise";		
		fi.m_nOrdinalPosition = 10;		
		td.CreateField(fi);
		// expl_cmt_rise
		fi.m_strName = "expl_cmt_rise";
		fi.m_nOrdinalPosition = 11;		
		td.CreateField(fi);
		// tect_cmt_rise
		fi.m_strName = "tect_cmt_rise";
		fi.m_nOrdinalPosition = 12;		
		td.CreateField(fi);
		// cond_desc_depth
		fi.m_strName = "cond_desc_depth";		
		fi.m_nOrdinalPosition = 13;				
		td.CreateField(fi);
		// expl_desc_depth
		fi.m_strName = "expl_desc_depth";		
		fi.m_nOrdinalPosition = 14;				
		td.CreateField(fi);
		// tect_desc_depth
		fi.m_strName = "tech_desc_depth";		
		fi.m_nOrdinalPosition = 15;				
		td.CreateField(fi);		
		// drill_rc
		fi.m_strName = "drill_rc";		
		fi.m_nOrdinalPosition = 19;				
		td.CreateField(fi);
		// inp_date
		fi.m_strName = "inp_date";	
		fi.m_nType = dbDate;
		fi.m_lSize = 0x8;
		fi.m_nOrdinalPosition = 3;		
		td.CreateField(fi);	
		// primary key		
		ii.m_strName = "pk_well";		
		ii.m_nFields = 1;
		ii.m_pFieldInfos = &ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);
		
		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createHorz_P(CDaoDatabase* pdb)
{
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[3];
	ifi[0].m_strName = "well";	 
	ifi[1].m_strName = "horz";
	ifi[2].m_strName = "plast";
	ifi[0].m_bDescending = ifi[1].m_bDescending = ifi[2].m_bDescending = 0;
	
	try
	{			
		td.Create(HORZ_P);
		// well
		fi.m_strName = "well";
		fi.m_nType = dbText;
		fi.m_lSize = 0x7;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbVariableField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 2;			
		fi.m_bRequired = 1;	
		fi.m_lAttributes = dbFixedField;
		td.CreateField(fi);		
		// xc
		fi.m_strName = "xc";			
		fi.m_nOrdinalPosition = 4;				
		td.CreateField(fi);
		// yc
		fi.m_strName = "yc";			
		fi.m_nOrdinalPosition = 5;				
		td.CreateField(fi);
		// plast
		fi.m_strName = "plast";	
		fi.m_nType = dbByte;	
		fi.m_lSize = 0x1;
		fi.m_strDefaultValue = "0";
		fi.m_nOrdinalPosition = 3;				
		td.CreateField(fi);	
		// roof_lngth
		fi.m_strName = "roof_lngth";
		fi.m_nType = dbSingle;		
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 10;	
		fi.m_bRequired = 0;	
		td.CreateField(fi);
		// sole_lngth
		fi.m_strName = "sole_lngth";		
		fi.m_nOrdinalPosition = 11;				
		td.CreateField(fi);
		// roof
		fi.m_strName = "roof";		
		fi.m_strDefaultValue = "";
		fi.m_nOrdinalPosition = 6;		
		td.CreateField(fi);
		// sole
		fi.m_strName = "sole";
		fi.m_nOrdinalPosition = 7;		
		td.CreateField(fi);
		// aroof
		fi.m_strName = "aroof";		
		fi.m_nOrdinalPosition = 8;		
		td.CreateField(fi);
		// asole
		fi.m_strName = "asole";		
		fi.m_nOrdinalPosition = 9;		
		td.CreateField(fi);			
		// vnk
		fi.m_strName = "vnk";		
		fi.m_nOrdinalPosition = 12;	
		fi.m_strDefaultValue = "-1";
		td.CreateField(fi);
		// avnk
		fi.m_strName = "avnk";
		fi.m_nOrdinalPosition = 13;		
		td.CreateField(fi);		
		// gnk
		fi.m_strName = "gnk";		
		fi.m_nOrdinalPosition = 14;		
		td.CreateField(fi);
		// agnk
		fi.m_strName = "agnk";
		fi.m_nOrdinalPosition = 15;		
		td.CreateField(fi);			
		// primary key		
		ii.m_strName = "pk_horzp";		
		ii.m_nFields = 3;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);
		
		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}			
	return bres;
}
bool createPlast_P(CDaoDatabase* pdb)
{
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[4];
	ifi[0].m_strName = "well";	 
	ifi[1].m_strName = "horz";
	ifi[2].m_strName = "plast";
	ifi[3].m_strName = "layer";
	ifi[0].m_bDescending = ifi[1].m_bDescending = 
						   ifi[2].m_bDescending = ifi[3].m_bDescending = 0;
	
	try
	{			
		td.Create(PLAST_P);
		// well
		fi.m_strName = "well";
		fi.m_nType = dbText;
		fi.m_lSize = 0x7;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbVariableField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 2;			
		fi.m_bRequired = 1;	
		fi.m_lAttributes = dbFixedField;
		td.CreateField(fi);				
		// plast
		fi.m_strName = "plast";	
		fi.m_nType = dbByte;	
		fi.m_lSize = 0x1;
		fi.m_strDefaultValue = "0";
		fi.m_nOrdinalPosition = 3;				
		td.CreateField(fi);	
		// layer
		fi.m_strName = "layer";					
		fi.m_nOrdinalPosition = 4;				
		td.CreateField(fi);	
		// hn
		fi.m_strName = "hn";	
		fi.m_strDefaultValue = "";
		fi.m_nOrdinalPosition = 9;		
		td.CreateField(fi);
		// corr_sl
		fi.m_strName = "corr_sl";
		fi.m_nOrdinalPosition = 14;	
		fi.m_bRequired = 0;
		td.CreateField(fi);	
		// lytho
		fi.m_strName = "lytho";
		fi.m_nOrdinalPosition = 15;	
		fi.m_bRequired = 0;
		td.CreateField(fi);	
		// roof
		fi.m_strName = "roof";
		fi.m_nType = dbSingle;		
		fi.m_lSize = 0x4;
		fi.m_bRequired = 1;
		fi.m_nOrdinalPosition = 5;					
		td.CreateField(fi);
		// sole
		fi.m_strName = "sole";		
		fi.m_nOrdinalPosition = 6;				
		td.CreateField(fi);
		// aroof
		fi.m_strName = "aroof";				
		fi.m_nOrdinalPosition = 7;		
		td.CreateField(fi);
		// asole
		fi.m_strName = "asole";
		fi.m_nOrdinalPosition = 8;		
		td.CreateField(fi);		
		// m
		fi.m_strName = "m";		
		fi.m_nOrdinalPosition = 10;	
		fi.m_strDefaultValue = "-1";
		td.CreateField(fi);			
		// s
		fi.m_strName = "s";		
		fi.m_nOrdinalPosition = 11;			
		td.CreateField(fi);
		// k
		fi.m_strName = "k";
		fi.m_nOrdinalPosition = 12;				
		td.CreateField(fi);		
		// cl
		fi.m_strName = "cl";		
		fi.m_nOrdinalPosition = 13;		
		td.CreateField(fi);		
		// primary key		
		ii.m_strName = "pk_plastp";		
		ii.m_nFields = 4;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);
		
		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}
	return bres;
}
bool createSlice_P(CDaoDatabase* pdb)
{
	bool bres = 1;
	
	CDaoTableDef td(pdb);
	CDaoFieldInfo fi;
	CDaoIndexInfo ii;
	CDaoIndexFieldInfo ifi[4];
	ifi[0].m_strName = "well";	 
	ifi[1].m_strName = "horz";
	ifi[2].m_strName = "plast";
	ifi[3].m_strName = "slice";
	ifi[0].m_bDescending = ifi[1].m_bDescending = 
						   ifi[2].m_bDescending = ifi[3].m_bDescending = 0;
	
	try
	{			
		td.Create(SLICE_P);
		// well
		fi.m_strName = "well";
		fi.m_nType = dbText;
		fi.m_lSize = 0x7;
		fi.m_nOrdinalPosition = 1;		
		fi.m_bRequired = 1;
		fi.m_lAttributes = dbVariableField;	
		fi.m_bAllowZeroLength = 0;
		td.CreateField(fi);
		// horz
		fi.m_strName = "horz";
		fi.m_nType = dbLong;
		fi.m_lSize = 0x4;
		fi.m_nOrdinalPosition = 2;			
		fi.m_bRequired = 1;	
		fi.m_lAttributes = dbFixedField;
		td.CreateField(fi);				
		// plast
		fi.m_strName = "plast";	
		fi.m_nType = dbByte;	
		fi.m_lSize = 0x1;
		fi.m_strDefaultValue = "0";
		fi.m_nOrdinalPosition = 3;				
		td.CreateField(fi);	
		// layer
		fi.m_strName = "slice";					
		fi.m_nOrdinalPosition = 4;				
		td.CreateField(fi);						
		// aroof
		fi.m_strName = "aroof";	
		fi.m_nType = dbSingle;
		fi.m_lSize = 0x4;
		fi.m_strDefaultValue = "";
		fi.m_nOrdinalPosition = 5;		
		td.CreateField(fi);
		// asole
		fi.m_strName = "asole";
		fi.m_nOrdinalPosition = 6;		
		td.CreateField(fi);		
		// m
		fi.m_strName = "m";		
		fi.m_nOrdinalPosition = 7;	
		fi.m_strDefaultValue = "-1";
		td.CreateField(fi);			
		// s
		fi.m_strName = "s";		
		fi.m_nOrdinalPosition = 8;			
		td.CreateField(fi);
		// kxy
		fi.m_strName = "kxy";
		fi.m_nOrdinalPosition = 9;				
		td.CreateField(fi);		
		// kz
		fi.m_strName = "kz";
		fi.m_nOrdinalPosition = 10;				
		td.CreateField(fi);		
		// cl
		fi.m_strName = "cl";		
		fi.m_nOrdinalPosition = 11;		
		td.CreateField(fi);		
		// primary key		
		ii.m_strName = "pk_plastp";		
		ii.m_nFields = 4;
		ii.m_pFieldInfos = ifi;
		ii.m_bPrimary = 1;	 
		ii.m_bIgnoreNulls = 0;		
		td.CreateIndex(ii);
		
		td.Append();
		td.Close();
	}
	catch (CDaoException* e)
	{			
		e->ReportError();
		e->Delete();
		bres = 0;
	}
	return bres;
}
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_CUT, OnClick)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}
CMainFrame::~CMainFrame()
{
}
void CMainFrame::OnClick()
{	
	TCHAR dbname[32], sql[1024];
	sprintf(dbname, "db%d.mdb", 223);
	DeleteFile(dbname);
	
	CDaoDatabase* pdb = new CDaoDatabase();
	
	try
	{		
		pdb->Create(dbname, dbLangCyrillic, 0x40);				
	}
	catch(CDaoException* e)
	{			
		e->Delete();
		pdb->Open(dbname, 1);
	}				
	createHorz(pdb);
	createPlast(pdb);
	createCont(pdb);
	createGrid(pdb);
	createWell(pdb);
	createHorz_P(pdb);
	createPlast_P(pdb);
	createSlice_P(pdb);
	// relations
	CDaoRelationFieldInfo rfi[2];	
	
	CDaoRelationInfo ri;
	ri.m_lAttributes = dbRelationUpdateCascade | dbRelationDeleteCascade;
	ri.m_pFieldInfos = rfi;
	// horz_plast
	rfi[0].m_strName = "code";
	rfi[0].m_strForeignName = HORZ;
	
	ri.m_strName = "rel1";
	ri.m_nFields = 1;
	ri.m_strTable = HORZ;
	ri.m_strForeignTable = PLAST;	
	pdb->CreateRelation(ri);
	// grids_plast
	rfi[0].m_strName = HORZ;
	rfi[0].m_strForeignName = HORZ;
	rfi[1].m_strName = "code";
	rfi[1].m_strForeignName = PLAST;
	
	ri.m_strName = "rel2";
	ri.m_nFields = 2;
	ri.m_strTable = PLAST;
	ri.m_strForeignTable = GRIDS;		
	pdb->CreateRelation(ri);
	// cont_plast
	rfi[0].m_strName = HORZ;
	rfi[0].m_strForeignName = HORZ;
	rfi[1].m_strName = "code";
	rfi[1].m_strForeignName = PLAST;
	
	ri.m_strName = "rel3";
	ri.m_nFields = 2;
	ri.m_strTable = PLAST;
	ri.m_strForeignTable = CONT;		
	pdb->CreateRelation(ri);
	// well_horzp
	rfi[0].m_strName = "name";
	rfi[0].m_strForeignName = WELL;	
	
	ri.m_strName = "rel4";
	ri.m_nFields = 1;
	ri.m_strTable = WELL;
	ri.m_strForeignTable = HORZ_P;		
	pdb->CreateRelation(ri);
	// plast_horzp
	rfi[0].m_strName = HORZ;
	rfi[0].m_strForeignName = HORZ;
	rfi[1].m_strName = "code";
	rfi[1].m_strForeignName = PLAST;
	
	ri.m_strName = "rel5";
	ri.m_nFields = 2;
	ri.m_strTable = PLAST;
	ri.m_strForeignTable = HORZ_P;		
	pdb->CreateRelation(ri);
	// well_plastp
	rfi[0].m_strName = "name";
	rfi[0].m_strForeignName = WELL;	
	
	ri.m_strName = "rel6";
	ri.m_nFields = 1;
	ri.m_strTable = WELL;
	ri.m_strForeignTable = PLAST_P;		
	pdb->CreateRelation(ri);
	// plast_plastp
	rfi[0].m_strName = HORZ;
	rfi[0].m_strForeignName = HORZ;
	rfi[1].m_strName = "code";
	rfi[1].m_strForeignName = PLAST;
	
	ri.m_strName = "rel7";
	ri.m_nFields = 2;
	ri.m_strTable = PLAST;
	ri.m_strForeignTable = PLAST_P;		
	pdb->CreateRelation(ri);
	// well_slicep
	rfi[0].m_strName = "name";
	rfi[0].m_strForeignName = WELL;	
	
	ri.m_strName = "rel8";
	ri.m_nFields = 1;
	ri.m_strTable = WELL;
	ri.m_strForeignTable = SLICE_P;		
	pdb->CreateRelation(ri);
	// plast_slicep
	rfi[0].m_strName = HORZ;
	rfi[0].m_strForeignName = HORZ;
	rfi[1].m_strName = "code";
	rfi[1].m_strForeignName = PLAST;
	
	ri.m_strName = "rel9";
	ri.m_nFields = 2;
	ri.m_strTable = PLAST;
	ri.m_strForeignTable = SLICE_P;		
	pdb->CreateRelation(ri);

	pdb->Close();
	delete pdb;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
