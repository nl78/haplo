/*
 * Copyright (C) 1998-2004 Nicolas LAURENT
 * This file is part of `Haplo'
 * 
 *
 * `Haplo'  is free software;  you can  redistribute  it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation;  either version 2  of the License, or
 * (at your option) any later version.
 *
 * `Haplo' is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 * 
 * You should have  received  a copy of the   GNU General Public  License
 * along with `Haplo'.  If not, write to  the
 *
 *                                        Free Software Foundation,  Inc.
 *                                        675 Mass Ave, Cambridge, MA
 *                                        02139, USA.
 *
 */
/*
 * $Id: cpp.h,v 1.6 2003/04/23 20:13:45 nicoo Exp $
 *
 *
 * Copyright (C) 1999, 2000, 2001 Nicolas LAURENT
 * This file is part of `Haplo'
 * 
 *
 * `Haplo'  is free software;  you can  redistribute  it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation;  either version 2  of the License, or
 * (at your option) any later version.
 *
 * `Haplo' is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 * 
 * You should have  received  a copy of the   GNU General Public  License
 * along with `Haplo'.  If not, write to  the
 *
 *                                        Free Software Foundation,  Inc.
 *                                        675 Mass Ave, Cambridge, MA
 *                                        02139, USA.
 *
 *
 */

#ifndef __HAPLO_HAPLO_CPP_H__
#define __HAPLO_HAPLO_CPP_H__

#ifdef __cplusplus
#	include <haplo/object.h>
#	include <haplo/func.h>


/**
 *
 */
template <class C> class haplo_object
{
private:
	static void _display(const C *c){ c->display();		}
	static void _delete(C *c)	{ delete c;		}
	static C *  _copy(const C *c)	{ return new C(*c);	}

public:
	static haplo_type_t type_register(const char *name) {
		return(HAPLO_REGISTER_TYPE(name, _display, _delete, _copy));
	}
};

#endif /* __cpluscplus */
#endif /* __HAPLO_HAPLO_CPP_H__ */

