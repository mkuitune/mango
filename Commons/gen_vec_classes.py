outfile = "cms_vec.h"
f = open(outfile, 'w')

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

def vecvecopmod(name, op, dim):
    if dim == 3:
        return "{0}& operator{1}(const {0}& s){{x {1} s.x; y {1} s.y; z {1} s.z; return *this;}}\n".format(name, op)
    else:
        return "{0}& operator{1}(const {0}& s){{x {1} s.x; y {1} s.y; z {1} s.z; w {1} s.w; return *this;}}\n".format(name, op)


def vecscalopmod(name, scaltyp, op, dim):
    if dim == 3:
        return "{0}& operator{2}(const {1}& s){{x {2} s; y {2} s; z {2} s; return *this;}}\n".format(name, scaltyp, op)
    else:
        return "{0}& operator{2}(const {1}& s){{x {2} s; y {2} s; z {2} s; w {2} s;return *this;}}\n".format(name, scaltyp, op)

def vecstr3(name, typ, postfix = ""):
    dim = 3
    res = "".join(["struct {0} {{\n".format(name),
        "{0} x, y, z;\n".format(typ),
        vecscalop(name, typ, '+', dim),
        vecscalop(name, typ, '-', dim),
        vecscalop(name, typ, '*', dim),
        vecscalop(name, typ, '/', dim),

        vecvecopmod(name, '+=', dim),
        vecvecopmod(name, '-=', dim),
        vecvecopmod(name, '*=', dim),
        vecvecopmod(name, '/=', dim),

        vecscalopmod(name, typ, '+=', dim),
        vecscalopmod(name, typ, '-=', dim),
        vecscalopmod(name, typ, '*=', dim),
        vecscalopmod(name, typ, '/=', dim),

        vecvecop(name, '+', dim),
        vecvecop(name, '-', dim),

        "template <typename T, typename Traits>",
		"friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const {}& v){{\n".format(name),
			"return out << v.x << \", \" << v.y << \", \" << v.z;};\n",
        "};\n"])
    return res

def vecstr4(name, typ, postfix = ""):
    dim = 4
    res = "".join(["struct {0} {{\n".format(name),
        "{0} x, y, z, w;\n".format(typ),
        vecscalop(name, typ, '+', dim),
        vecscalop(name, typ, '-', dim),
        vecscalop(name, typ, '*', dim),
        vecscalop(name, typ, '/', dim),

        vecvecopmod(name, '+=', dim),
        vecvecopmod(name, '-=', dim),
        vecvecopmod(name, '*=', dim),
        vecvecopmod(name, '/=', dim),

        vecscalopmod(name, typ, '+=', dim),
        vecscalopmod(name, typ, '-=', dim),
        vecscalopmod(name, typ, '*=', dim),
        vecscalopmod(name, typ, '/=', dim),

        vecvecop(name, '+', dim),
        vecvecop(name, '-', dim),

        "template <typename T, typename Traits>",
		"friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const {}& v){{\n".format(name),
			"return out << v.x << \", \" << v.y << \", \" << v.z << ", " << v.w;};\n",
        "};\n"])
    return res


f.write("#pragma once\n")
f.write("#include <iostream>\n")
f.write("namespace cms {\n")

f.write(vecstr3("Vec3f", "float", "f"))
f.write("\n")
f.write(vecstr3("Vec3", "double"))
f.write("\n")

f.write(vecstr4("Vec4f", "float", "f"))
f.write("\n")
f.write(vecstr4("Vec4", "double"))
f.write("\n")


f.write("}\n")
f.close()
