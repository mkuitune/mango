outfile = "cms_vec.h"
f = open(outfile, 'w')

vec4 = {'name':"Vec4", 'type':"double", 'num_prefix':""}

op4_vec_scal_template = "VEC operatorOP(const VEC& v, const SCALAR& s)"

#0:name 1:type 2:prefix
def vecop(name, scaltyp, op, dim):
    if dim == 3:
        return "{0} operator{2}(const {1}& s){{return {{x {2} s, y {2} s, z {2} s}};}}\n".format(name, scaltyp, op)
    else:
        return "{0} operator{2}(const {1}& s){{return {{x {2} s, y {2} s, z {2} s, w {2} s}};}}\n".format(name, scaltyp, op)

def vecstr3(name, typ, postfix = ""):
    res = "".join(["struct {0} {{\n".format(name),  "{0} x, y, z;\n".format(typ), vecop(name, typ, '+', 3),
    "};\n"])
    return res

f.write("#pragma once\n")
f.write("namespace cms {\n")

f.write(vecstr3("Vec3f", "float", "f"))

f.write("}\n")
f.close()
