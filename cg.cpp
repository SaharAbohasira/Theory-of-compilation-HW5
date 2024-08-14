#include "cg.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include "general.h"
#include "symbol_table.h"

using namespace std;

extern general codeGenerator;

CodeBuffer::CodeBuffer() : buffer(), globalDefs() {}

CodeBuffer &CodeBuffer::instance() {
	static CodeBuffer inst;//only instance
	return inst;
}

string CodeBuffer::freshLabel(){
	std::stringstream label;
	label << "label_" << ++labels_num;
	return label.str();
}

int CodeBuffer::emit(const string &s){
    buffer.push_back(s);
	return buffer.size() - 1;
}

void CodeBuffer::printCodeBuffer(){
	for (std::vector<string>::const_iterator it = buffer.begin(); it != buffer.end(); ++it) 
	{
		cout << *it << endl;
    }
}

// ******** Methods to handle the global section ********** //
void CodeBuffer::emitGlobal(const std::string& dataLine) 
{
	globalDefs.push_back(dataLine);
}

void CodeBuffer::printGlobalBuffer()
{
	for (vector<string>::const_iterator it = globalDefs.begin(); it != globalDefs.end(); ++it)
	{
		cout << *it << endl;
	}
}

void CodeBuffer::declareFuncs()
{
    emit("@.intFormat = internal constant [4 x i8] c\"%d\\0A\\00\"");
    emit("@.DIVIDE_BY_ZERO.str = internal constant [23 x i8] c\"Error division by zero\\00\"");
    emit("declare i32 @printf(i8*, ...)");
    emit("declare void @exit(i32)");
    emit("declare i32 @scanf(i8*, ...)");
    emit("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    emit("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    emit("@.int_specifier_scan = constant [3 x i8] c\"%d\\00\"");
}

void CodeBuffer::printDefinitions()
{
    emit("define void @print(i8*){");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    emit("ret void");
    emit("}");
    emit("define void @printi(i32){");
    emit("%format_ptr = getelementptr [4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.intFormat, i32 0, i32 0), i32 %0)");
    emit("ret void");
    emit("}");
}

void CodeBuffer::initialEmits()
{
    codeGenerator.globalCode();
    declareFuncs();
    printDefinitions();
}