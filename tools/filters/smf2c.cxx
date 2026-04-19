/************************************************************************

  smf2c

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: smf2c.cxx,v 1.1 1998/10/27 01:23:43 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include "cmdline.h"

static
void output_c(MxStdModel *m, ostream& out)
{
    uint i;

    out << "static" << endl
	<< "float v[][3] = {" << endl;

    for(i=0; i<m->vert_count(); i++)
    {
	out << "    {" << m->vertex(i)[X]
	    << ", "    << m->vertex(i)[Y]
	    << ", "    << m->vertex(i)[Z]
	    << "}," << endl;
    }
    out << "};" << endl;

    if( m->normal_binding() == MX_PERVERTEX )
    {
	out << endl;
	out << "static" << endl
	    << "float n[][3] = {" << endl;
	for(i=0; i<m->normal_count(); i++)
	{
	    out << "    {" << m->normal(i)[X]
		<< ", "    << m->normal(i)[Y]
		<< ", "    << m->normal(i)[Z]
		<< "}," << endl;
	}
	out << "};" << endl;
    }

    out << endl;
    out << "static" << endl
	<< "unsigned int f[][3] = {" << endl;
    for(i=0; i<m->face_count(); i++)
    {
	out << "    {" << m->face(i)[0]
	    << ", "    << m->face(i)[1]
	    << ", "    << m->face(i)[2]
	    << "}," << endl;
    }
    out << "};" << endl;
    
    out << endl;
    out << "glBegin(GL_TRIANGLES);" << endl;
    out << "  for(uint i=0; i<" << m->face_count() << "; i++)" << endl;
    out << "  {" << endl;
    if( m->normal_binding() == MX_PERVERTEX )
	out << "      glNormal3fv(n[f[i][0]]);" << endl;
    out << "      glVertex3fv(v[f[i][0]]);" << endl;
    if( m->normal_binding() == MX_PERVERTEX )
	out << "      glNormal3fv(n[f[i][1]]);" << endl;
    out << "      glVertex3fv(v[f[i][1]]);" << endl;
    if( m->normal_binding() == MX_PERVERTEX )
	out << "      glNormal3fv(n[f[i][2]]);" << endl;
    out << "      glVertex3fv(v[f[i][2]]);" << endl;
    out << "  }" << endl;
    out << "glEnd();" << endl;
}

int main(int argc, char *argv[])
{
    MxStdModel *m = process_cmdline(argc, argv);
    if( !m ) return 1;

    output_c(m, cout);
    delete m;
    return 0;
}
