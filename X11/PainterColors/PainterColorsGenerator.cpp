#include <bits/stdc++.h>
#include <boost/regex.hpp>

using namespace std;

vector<string> names;

string to_slug(const string & name)
{
	string slug;
	for(char c : name)
	{
		if(c == ' ')
			slug += '_';
		else if(('a' <= c and c <= 'z') or ('A' <= c and c <= 'Z') or ('0' <= c and c <= '9'))
			slug += c;
		else
			throw 1;
	}
	return slug;
}

void generate_code(ofstream & stream)
{
	stream << "#include \"PainterColors.h\"\n";
	stream << "#include <X11/Xlib.h>\n";
	stream << "\n";
	stream << "PainterColors::PainterColors(void * display)\n";
	stream << "{\n";
	stream << "\tDisplay * dis = (Display*) display;\n";
	stream << "\tXColor tmp;\n";
	stream << "\n";
	for(string name : names)
	{
		stream << "\tXParseColor(dis, DefaultColormap(dis, DefaultScreen(dis)), \"" << name << "\", &tmp);\n";
		stream << "\tXAllocColor(dis, DefaultColormap(dis, DefaultScreen(dis)), &tmp);\n";
		stream << "\t" << to_slug(name) << " = tmp.pixel;\n";
	}
	stream << "}\n";
}

void generate_header(ofstream & stream)
{
	stream << "#pragma once\n";
	stream << "\n";
	stream << "struct PainterColors\n";
	stream << "{\n";
	stream << "\tPainterColors(void * display);\n";
	stream << "\n";
	stream << "\tunsigned long ";
	bool first = true;
	for(string name : names)
	{
		if(!first)
			stream << ", ";
		else
			first = false;
		stream << to_slug(name);
	}
	stream << ";\n";
	stream << "};\n";
}

void add_name(const string & name)
{
	names.emplace_back(name);
}

void process_line(const string & line)
{
	static boost::regex r("^\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\w.*\\w)\\s*$");
	boost::smatch match;
	if(boost::regex_search(line, match, r))
		add_name(match[4]);
}

int main(int argc, char ** argv)
{
	if(argc != 4)
	{
		cerr << "Usage: " << argv[0] << " PainterColorsGenerator.rgb.txt PainterColors.cpp PainterColors.h\n";
		return 1;
	}
	ifstream f(argv[1]);
	if(f.fail())
	{
		cerr << "Failed to open " << argv[1] << "\n";
		return 1;
	}
	string s;
	while(true)
	{
		getline(f, s);
		if(f.eof())
			break;
		process_line(s);
	}
	{ ofstream code(argv[2]); generate_code(code); }
	{ ofstream header(argv[3]); generate_header(header); }
	return 0;
}
