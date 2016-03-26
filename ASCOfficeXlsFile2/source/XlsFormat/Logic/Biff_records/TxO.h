#pragma once

#include "BiffRecordContinued.h"
#include <Logic/Biff_structures/ControlInfo.h>
#include <Logic/Biff_structures/FontIndex.h>
#include <Logic/Biff_structures/ObjFmla.h>
#include <Logic/Biff_structures/TxORuns.h>
#include <Logic/Biff_records/MsoDrawing.h>

namespace XLS
{


// Logical representation of TxO record in BIFF8
class TxO: public BiffRecordContinued
{
	BIFF_RECORD_DEFINE_TYPE_INFO(TxO)
	BASE_OBJECT_DEFINE_CLASS_NAME(TxO)
public:
	TxO(MsoDrawingPtr mso_drawing) : mso_drawing_(mso_drawing), fmla(false)
	{
		macrosSize			= 0;
		nameLength			= 0;
		cbRuns				= 0;
		sp_enabled			= false;
		preserve_enabled	= false;
	}
	~TxO();

	BaseObjectPtr clone();

	void writeFields(CFRecord& record);
	void readFields(CFRecord& record);

	static const ElementType	type = typeTxO;

	int serialize		(std::wostream & _stream);	
	int serialize_rPr	(std::wostream & _stream, int iFmt, std::wstring namespace_= L"a:");

	GlobalWorkbookInfoPtr	pGlobalWorkbookInfoPtr;

	unsigned char			hAlignment;
	unsigned char			vAlignment;

	unsigned char			fLockText;
	unsigned char			fJustLast;
	unsigned char			fSecretEdit;

	_UINT16					rot;

	ControlInfo				controlInfo;

	_UINT16					cchText;
	_UINT16					cbRuns;
	FontIndex				ifntEmpty;
	ObjFmla					fmla;
	TxORuns					TxOruns;
	XLUnicodeStringNoCch	rawText;

	bool					sp_enabled;

	MsoDrawingPtr			mso_drawing_;


//---------------------------------------------
	bool					preserve_enabled;
	short					macrosSize;
	short					nameLength;
	ShortXLAnsiString		name;
};

} // namespace XLS

