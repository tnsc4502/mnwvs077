#include "MovePath.h"

#include <..\WvsLib\Net\InPacket.h>
#include <..\WvsLib\Net\OutPacket.h>

MovePath::MovePath()
{
}


MovePath::~MovePath()
{
}

void MovePath::Decode(InPacket * iPacket)
{
	m_x = iPacket->Decode2();
	m_y = iPacket->Decode2();
	int nCount = iPacket->Decode1();
	int nOX = 0, nOY = 0;
	for (int i = 0; i < nCount; ++i)
	{
		ELEM elem;
		elem.nAttr = iPacket->Decode1();
		switch (elem.nAttr)
		{
			case 0:
			case 5:
			case 0xF:
			case 0x11: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.fh = iPacket->Decode2();
				if (elem.nAttr == 15)
					elem.fhFootStart = iPacket->Decode2();
				break;
			}
			case 1:
			case 2:
			case 6:
			case 0x0C:
			case 0x0D:
			case 0x10: {
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.x = nOX;
				elem.y = nOY;
				break;
			}
			case 3:
			case 4:
			case 7:
			case 8:
			case 9:
			case 0x0B: {
				elem.x = iPacket->Decode2();
				elem.y = iPacket->Decode2();
				elem.fh = iPacket->Decode2();
				break;
			}
			case 0x0E: {
				elem.vx = iPacket->Decode2();
				elem.vy = iPacket->Decode2();
				elem.fhFootStart = iPacket->Decode2();
				break;
			}
			case 0x0A:
			{
				elem.bStat = iPacket->Decode1();
				break;
			}
		}
		if (elem.nAttr != 0x0A)
		{
			elem.bMoveAction = iPacket->Decode1();
			elem.tElapse = iPacket->Decode2();
			nOX = elem.x;
			nOY = elem.y;
		}
		m_lElem.push_back(std::move(elem));
	}
}

void MovePath::Encode(OutPacket * oPacket)
{
	oPacket->Encode2(m_x);
	oPacket->Encode2(m_y);
	oPacket->Encode1((char)m_lElem.size());
	for (const auto& elem : m_lElem)
	{
		oPacket->Encode1(elem.nAttr);
		switch (elem.nAttr)
		{
			case 0:
			case 5:
			case 0xF:
			case 0x11: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				oPacket->Encode2(elem.fh);

				if (elem.nAttr == 15)
					oPacket->Encode2(elem.fhFootStart);

				m_fhLast = elem.fh;
				break;
			}
			case 1:
			case 2:
			case 6:
			case 0x0C:
			case 0x0D:
			case 0x10: {
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				break;
			}

			case 3:
			case 4:
			case 7:
			case 8:
			case 9:
			case 0x0B: {
				oPacket->Encode2(elem.x);
				oPacket->Encode2(elem.y);
				oPacket->Encode2(elem.fh);
				m_fhLast = elem.fh;
				break;
			}
			case 0x0E: {
				oPacket->Encode2(elem.vx);
				oPacket->Encode2(elem.vy);
				oPacket->Encode2(elem.fhFootStart);
				break;
			}
			case 0x0A: {
				oPacket->Encode1(elem.bStat);
				break;
			}
		}
		if (elem.nAttr != 0x0A)
		{
			oPacket->Encode1(elem.bMoveAction);
			oPacket->Encode2(elem.tElapse);
		}
	}
}
