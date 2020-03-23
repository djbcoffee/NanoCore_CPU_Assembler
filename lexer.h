/*
 ********************************************************************************
 ** Copyright (C) 2018 Donald J. Bartley <djbcoffee@gmail.com>
 **
 ** This source file may be used and distributed without restriction provided
 ** that this copyright statement is not removed from the file and that any
 ** derivative work contains the original copyright notice and the associated
 ** disclaimer.
 **
 ** This source file is free software; you can redistribute it and/or modify it
 ** under the terms of the GNU General Public License as published by the Free
 ** Software Foundation; either version 2 of the License, or (at your option) any
 ** later version.
 **
 ** This source file is distributed in the hope that it will be useful, but
 ** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 ** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 ** more details.
 **
 ** You should have received a copy of the GNU General Public License along with
 ** this source file.  If not, see <http://www.gnu.org/licenses/> or write to the
 ** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 ** 02110-1301, USA.
 ********************************************************************************
 ** File: nanocore-as/src/lexer.h
 **
 ** Description:
 ** Header file that goes with lexer.c
 ********************************************************************************
 ** Version 1.0.0
 ********************************************************************************
 ** DJB 11/21/18 Created.
 ********************************************************************************
 */

#ifndef ___LEXER_H___
#define ___LEXER_H___

//------------------------------------------------------------------------------
//Defines
#define MAX_PROGRAM_MEMORY              (65536)

#define MAX_SYMBOL_SIZE                 (255)
#define MAX_DIR_MNEMONIC_SIZE           (4)
#define MAX_OPERANDS                    (1)

#define BYTE_DIRECTIVE_SUCCESS          (1)
#define END_DIRECTIVE_SUCCESS           (2)
#define EQU_DIRECTIVE_SUCCESS           (3)
#define ORG_DIRECTIVE_SUCCESS           (4)

#define BYTE_DIRECTIVE_TEXT             "BYTE"
#define END_DIRECTIVE_TEXT              "END"
#define EQU_DIRECTIVE_TEXT              "EQU"
#define ORG_DIRECTIVE_TEXT              "ORG"

#define ADD_INSTRUCTION_TEXT            "ADD"
#define AND_INSTRUCTION_TEXT            "AND"
#define BCC_INSTRUCTION_TEXT            "BCC"
#define BCS_INSTRUCTION_TEXT            "BCS"
#define BZC_INSTRUCTION_TEXT            "BZC"
#define BZS_INSTRUCTION_TEXT            "BZS"
#define CLC_INSTRUCTION_TEXT            "CLC"
#define DDP_INSTRUCTION_TEXT            "DDP"
#define IDP_INSTRUCTION_TEXT            "IDP"
#define JMP_INSTRUCTION_TEXT            "JMP"
#define JSR_INSTRUCTION_TEXT            "JSR"
#define LDA_INSTRUCTION_TEXT            "LDA"
#define LDP_INSTRUCTION_TEXT            "LDP"
#define ORA_INSTRUCTION_TEXT            "ORA"
#define ROL_INSTRUCTION_TEXT            "ROL"
#define ROR_INSTRUCTION_TEXT            "ROR"
#define RTS_INSTRUCTION_TEXT            "RTS"
#define SEC_INSTRUCTION_TEXT            "SEC"
#define STA_INSTRUCTION_TEXT            "STA"
#define SUB_INSTRUCTION_TEXT            "SUB"
#define XOR_INSTRUCTION_TEXT            "XOR"

#define NUMBER_OF_ADD_TYPES             (2)
#define NUMBER_OF_AND_TYPES             (2)
#define NUMBER_OF_BCC_TYPES             (1)
#define NUMBER_OF_BCS_TYPES             (1)
#define NUMBER_OF_BZC_TYPES             (1)
#define NUMBER_OF_BZS_TYPES             (1)
#define NUMBER_OF_CLC_TYPES             (1)
#define NUMBER_OF_DDP_TYPES             (1)
#define NUMBER_OF_IDP_TYPES             (1)
#define NUMBER_OF_JMP_TYPES             (1)
#define NUMBER_OF_JSR_TYPES             (1)
#define NUMBER_OF_LDA_TYPES             (4)
#define NUMBER_OF_LDP_TYPES             (2)
#define NUMBER_OF_ORA_TYPES             (2)
#define NUMBER_OF_ROL_TYPES             (1)
#define NUMBER_OF_ROR_TYPES             (1)
#define NUMBER_OF_RTS_TYPES             (1)
#define NUMBER_OF_SEC_TYPES             (1)
#define NUMBER_OF_STA_TYPES             (3)
#define NUMBER_OF_SUB_TYPES             (2)
#define NUMBER_OF_XOR_TYPES             (2)

//------------------------------------------------------------------------------
//Enumerations
enum Passes
{
    PassOne = 1,
    PassTwo
};

enum DirectiveIndexes
{
    ByteDirective = 0,  //Must start at zero to match first index of array.
    EndDirective,
    EquDirective,
    OrgDirective
};

enum InstructionIndexes
{
    AddInstruction = 0, //Must start at zero to match first index of array.
    AndInstruction,
    BccInstruction,
    BcsInstruction,
    BzcInstruction,
    BzsInstruction,
    ClcInstruction,
    DdpInstruction,
    IdpInstruction,
    JmpInstruction,
    JsrInstruction,
    LdaInstruction,
    LdpInstruction,
    OraInstruction,
    RolInstruction,
    RorInstruction,
    RtsInstruction,
    SecInstruction,
    StaInstruction,
    SubInstruction,
    XorInstruction
};

enum InstructionTypes
{
    Implied,
    Immediate,
    DirectPage,
    DirectPageIndirect,
    Absolute
};

enum GetExpressionStates
{
    CheckForExpression = 0,
    CheckForComma
};

//------------------------------------------------------------------------------
//Structures
typedef struct tagDirectiveInfo
{
    uint32_t m_iIndex;
    char* m_cpDirective;
} DirectiveInfo;

typedef struct tagExpressionInfo
{
    char* m_cpStart;
    int m_iValue;
} ExpressionInfo;

typedef struct tagOpcodeInfo
{
    uint8_t m_iOpCode;
    uint8_t m_iType;
    uint8_t m_iNumberOfOperands;
    uint8_t m_iLength;
} OpcodeInfo;

typedef struct tagInstructionInfo
{
    uint32_t m_iIndex;
    char* m_cpMnemonic;
    uint8_t m_iNumberOfTypes;
    const OpcodeInfo* m_pOpcodeInfo;
} InstructionInfo;

typedef struct tagLexerInfo
{
    char* m_cpStartOfLabel;
    char* m_cpEndOfLabel;
    char* m_cpStartOfStatement;
    char* m_cpEndOfStatement;
    char* m_cpStartOfComment;
    char* m_cpEndOfComment;
    char* m_cpStatementSymbolStart;
    char* m_cpStatementSymbolEnd;
    char* m_cpStatementMnemonicStart;
    char* m_cpStatementMnemonicEnd;
    char* m_cpStatementExpresionStart;
    uint8_t m_iPass;
    int16_t m_iaProgramMemory[MAX_PROGRAM_MEMORY];
    uint32_t m_iLargestSymbolLength;
    uint32_t m_iLargestOperandLength;
    uint32_t m_iSourceLineLength;
    uint32_t m_iSourceLineNumber;
    uint32_t m_iLocationCounter;
} LexerInfo;

//------------------------------------------------------------------------------
//Prototypes
int do_assembly(void);
void free_lexer_memory(void);
uint32_t get_location_counter_value(void);
BSTreeNode* get_symbol_table_root(void);

#endif /*___LEXER_H___*/
