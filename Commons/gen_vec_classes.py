outfile = "cms_vec.h"
f = open(outfile, 'w')

vec4 = {'name':"Vec4", 'type':"double", 'num_prefix':""}

op4_vec_scal_template = "VEC operatorOP(const VEC& v, const SCALAR& s)"

#0:name 1:type 2:prefix
def vecscalop(name, scaltyp, op, dim):
    if dim == 3:
        return "{0} operator{2}(const {1}& s) const {{return {{x {2} s, y {2} s, z {2} s}};}}\n".format(name, scaltyp, op)
    else:
        return "{0} operator{2}(const {1}& s) const {{return {{x {2} s, y {2} s, z {2} s, w {2} s}};}}\n".format(name, scaltyp, op)

def vecvecop(name, op, dim):
    if dim == 3:
        return "{0} operator{1}(const {0}& s) const {{return {{x {1} s.x, y {1} s.y, z {1} s.z}};}}\n".format(name, op)
    else:
        return "{0} operator{1}(const {0}& s) const {{return {{x {1} s.x, y {1} s.y, z {1} s.z, w {1} s.w}};}}\n".format(name, op)

def vecscalopmod(name, scaltyp, op, dim):
    if dim == 3:
        return "{0}& operator{2}(const {1}& s){{x {2} s; y {2} s; z {2} s; return *this;}}\n".format(name, scaltyp, op)
    else:
        return "{0}& operator{2}(const {1}& s){{x {2} s; y {2} s; z {2} s; w {2} s;return *this;}}\n".format(name, scaltyp, op)

def vecstr3(name, typ, postfix = ""):
    res = "".join(["struct {0} {{\n".format(name),
        "{0} x, y, z;\n".format(typ),
        vecscalop(name, typ, '+', 3),
        vecscalop(name, typ, '-', 3),
        vecscalop(name, typ, '*', 3),
        vecscalop(name, typ, '/', 3),

        vecscalopmod(name, typ, '+=', 3),
        vecscalopmod(name, typ, '-=', 3),
        vecscalopmod(name, typ, '*=', 3),
        vecscalopmod(name, typ, '/=', 3),

        vecvecop(name, '+', 3),
        vecvecop(name, '-', 3),

        "template <typename T, typename Traits>",
		"friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const {}& v){{\n".format(name),
			"return out << v.x << \", \" << v.y << \", \" << v.z;};\n",
        "};\n"])
    return res

f.write("#pragma once\n")
f.write("#include <iostream>\n")
f.write("namespace cms {\n")

f.write(vecstr3("Vec3f", "float", "f"))

f.write("\n")
f.write(vecstr3("Vec3", "double"))

f.write("}\n")
f.close()
