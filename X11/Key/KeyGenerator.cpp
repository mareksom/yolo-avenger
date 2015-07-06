#include <bits/stdc++.h>

#include <boost/regex.hpp>

using namespace std;

set<string> tokens;
set<string> boolTokens;

vector< pair<string, string> > bindings;
vector< pair<string, std::vector<string> > > groups;
vector< pair< pair<string, string>, vector<string> > > sequences;

void readInput(const string & path)
{
	auto strip_whitespaces = [] (const string & text)
	{
		static boost::regex whitespaces("\\s+");
		static boost::regex begin_whitespaces("^\\s+");
		static boost::regex end_whitespaces("\\s+$");
		const string & a = boost::regex_replace(text, whitespaces, " ");
		const string & b = boost::regex_replace(a, begin_whitespaces, "");
		const string & c = boost::regex_replace(b, end_whitespaces, "");
		return c;
	};

	auto split_to_words = [&] (const string & text)
	{
		string s = strip_whitespaces(text);
		vector<string> v;
		string acc;
		for(char c : s)
		{
			if(c == ' ')
			{
				if(acc.length() > 0)
				{
					v.push_back(acc);
					acc = "";
				}
			}
			else
				acc += c;
		}
		if(acc.length() > 0)
			v.push_back(acc);
		return v;
	};

	auto process_line = [&] (const string & line)
	{
		static boost::regex key("^\\s*Key\\s+(\\w+)\\s+(\\w+)\\s*$");
		static boost::regex group("^\\s*Group\\s+(\\w+)((\\s+\\w+)+\\s*)$");
		static boost::regex sequence("^\\s*Sequence\\s+(\\w+)\\s+(\\d+)((\\s+\\w+)+)\\s*$");
		boost::smatch match;
		if(boost::regex_search(line, match, key))
		{
			if(tokens.find(match[1]) != tokens.end())
			{
				cerr << "Line '" << line << "', token exists.\n";
				exit(1);
			}
			tokens.insert(match[1]);
			boolTokens.insert(match[1]);
			bindings.emplace_back(match[1], match[2]);
		}
		else if(boost::regex_search(line, match, group))
		{
			if(tokens.find(match[1]) != tokens.end())
			{
				cerr << "Line '" << line << "', token exists.\n";
				exit(1);
			}
			vector<string> v = split_to_words(match[2]);
			for(string s : v)
			{
				if(tokens.find(s) == tokens.end())
				{
					cerr << "Line '" << line << "', group token does not exist.\n";
					exit(1);
				}
			}
			groups.emplace_back(match[1], v);
			tokens.insert(match[1]);
			boolTokens.insert(match[1]);
		}
		else if(boost::regex_search(line, match, sequence))
		{
			if(tokens.find(match[1]) != tokens.end())
			{
				cerr << "Line '" << line << "', token exists.\n";
				exit(1);
			}
			tokens.insert(match[1]);
			vector<string> v = split_to_words(match[3]);
			for(string s : v)
			{
				if(boolTokens.find(s) == boolTokens.end())
				{
					cerr << "Line '" << line << "', sequence boolToken does not exist.\n";
					exit(1);
				}
			}
			sequences.emplace_back(make_pair(match[1], match[2]), v);
		}
	};

	ifstream list(path);
	if(list.fail())
	{
		cerr << "Failed to open file: " << path << "\n";
		exit(1);
	}
	while(true)
	{
		string line;
		getline(list, line);
		if(list.eof())
			break;
		process_line(line);
	}
}

void printHeader(const string & path)
{
	ofstream out(path);
	out << "#pragma once\n";
	out << "\n";
	out << "#include <iostream>\n";
	out << "\n";
	out << "class Key\n";
	out << "{\n";
	out << "public:\n";
	out << "\tKey(void * eventPtr);\n";
	out << "\n";
	out << "\tunsigned long value() const;\n";
	out << "\n";
	out << "\tchar getChar() const;\n";
	out << "\n";
	out << "\tbool modCtrl() const;\n";
	out << "\tbool modShift() const;\n";
	out << "\n";
	for(auto & b : bindings)
		out << "\tbool " << b.first << "() const;\n";
	out << "\n";
	for(auto & g : groups)
		out << "\tbool " << g.first << "() const;\n";
	out << "\n";
	for(auto & s : sequences)
		out << "\tint " << s.first.first << "() const;\n";
	out << "\n";
	out << "\tfriend std::ostream & operator << (std::ostream & stream, const Key & key);\n";
	out << "\n";
	out << "private:\n";
	out << "\tunsigned long key;\n";
	out << "\tunsigned long mask;\n";
	out << "\tchar character;\n";
	out << "};\n";
}

void printSource(const string & path)
{
	ofstream out(path);
	out << "#include \"Key.h\"\n";
	out << "\n";
	out << "#include <X11/Xlib.h>\n";
	out << "#include <X11/Xutil.h>\n";
	out << "\n";
	out << "Key::Key(void * eventPtr)\n";
	out << "{\n";
	//out << "\tchar buffer[2];\n";
	out << "\tXKeyEvent * event = (XKeyEvent*) eventPtr;\n";
	out << "\tif(XLookupString(event, &character, 1, &key, nullptr) != 1)\n";
	out << "\t\tcharacter = 0;\n";
	out << "\tmask = event->state;\n";
	out << "}\n";
	out << "\n";
	out << "unsigned long Key::value() const { return key; }\n";
	out << "\n";
	out << "char Key::getChar() const { return character; }\n";
	out << "\n";
	out << "bool Key::modCtrl() const { return mask & ControlMask; }\n";
	out << "bool Key::modShift() const { return mask & ShiftMask; }\n";
	out << "\n";
	for(auto & b : bindings)
		out << "bool Key::" << b.first << "() const { return key == XK_" << b.second << "; }\n";
	out << "\n";
	for(auto & g : groups)
	{
		out << "bool Key::" << g.first << "() const { return ";
		bool first = true;
		for(auto & t : g.second)
		{
			if(!first)
				out << " or ";
			else
				first = false;
			out << t << "()";
		}
		out << "; }\n";
	}
	out << "\n";
	for(auto & s : sequences)
	{
		auto & v = s.second;
		out << "int Key::" << s.first.first << "() const\n";
		out << "{\n";
		for(int i = 0; i < (int) v.size(); i++)
		{
			out << "\t";
			if(i)
				out << "else ";
			out << "if(" << v[i] << "())\n";
			out << "\t\treturn " << s.first.second << " + " << i << ";\n";
		}
		out << "\treturn " << s.first.second << " + " << v.size() << ";\n";
		out << "}\n";
	}
	out << "\n";
	out << "std::ostream & operator << (std::ostream & stream, const Key & key)\n";
	out << "{\n";
	out << "\tstream << \"Key(\";\n";

	out << "\tbool first = true;\n";

	out << "\tif(key.getChar())\n";
	out << "\t{\n";
	out << "\t\tstream << \"'\" << key.getChar() << \"'\";\n";
	out << "\t\tfirst = false;\n";
	out << "\t}\n";

	auto make_booltoken_function = [&out] (const string & s) {
		out << "\tif(key." << s << "())\n";
		out << "\t{\n";
		out << "\t\tif(!first)\n";
		out << "\t\t\tstream << \", \";\n";
		out << "\t\telse\n";
		out << "\t\t\tfirst = false;\n";
		out << "\t\tstream << \"" << s << "\";\n";
		out << "\t}\n";
	};

	for(auto & t : boolTokens)
		make_booltoken_function(t);

	out << "\n";

	make_booltoken_function("modCtrl");
	make_booltoken_function("modShift");

	out << "\tstream << \")\";\n";
	out << "\treturn stream;\n";
	out << "}\n";
}

int main(int argc, char ** argv)
{
	if(argc != 4)
	{
		cerr << "Usage: " << argv[0] << " KeyGenerator.list Key.cpp Key.h\n";
		return 1;
	}
	readInput(argv[1]);
	printSource(argv[2]);
	printHeader(argv[3]);
	return 0;
}
