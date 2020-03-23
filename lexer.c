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
 ** File: nanocore-as/src/lexer.c
 **
 ** Description:
 ** This translation (compilation) unit contains the functions for parsing the
 ** lines of the source file and doing lexical scanning.
 ********************************************************************************
 ** Version 1.0.0
 ********************************************************************************
 ** DJB 11/21/18 Created.
 ********************************************************************************
 */

//System #includes
#ifndef _WINDOWS_H
#include <windows.h>
#endif
#ifndef _STDIO_H
#include <stdio.h>
#endif
#ifndef _STDINT_H
#include <stdint.h>
#endif
#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _CTYPE_H_
#include <ctype.h>
#endif
#ifndef _TIME_H
#include <time.h>
#endif

//Project-wide #includes
#ifndef ___UNIVERSAL_H___
#include "universal.h"
#endif

//Project #includes
#ifndef ___ARGUMENTS_H___
#include "arguments.h"
#endif
#ifndef ___BSTREE_H___
#include "bstree.h"
#endif
#ifndef ___EXPRESSION_H___
#include "expression.h"
#endif
#ifndef ___FILES_H___
#include "files.h"
#endif
#ifndef ___LOG_H___
#include "log.h"
#endif

//Reflective #includes
#ifndef ___LEXER_H___
#include "lexer.h"
#endif

//------------------------------------------------------------------------------
//Global Data
//None

//------------------------------------------------------------------------------
//Static Data
static char* s_cpSourceLine = NULL;

static uint32_t s_iStatementSymbolFieldLength;

static LexerInfo s_lexerInfo;

static BSTreeNode* s_pSymbolTableRoot = NULL;

static const DirectiveInfo s_aDirectiveTable[] =
{
    {ByteDirective, BYTE_DIRECTIVE_TEXT},
    {EndDirective, END_DIRECTIVE_TEXT},
    {EquDirective, EQU_DIRECTIVE_TEXT},
    {OrgDirective, ORG_DIRECTIVE_TEXT}
};

static const OpcodeInfo s_aAddOpcodeInfo[NUMBER_OF_ADD_TYPES] =
{
    {0x10, Immediate, 1, 2},
    {0x00, DirectPage, 1, 2}
};

static const OpcodeInfo s_aAndOpcodeInfo[NUMBER_OF_AND_TYPES] =
{
    {0x12, Immediate, 1, 2},
    {0x04, DirectPage, 1, 2}
};

static const OpcodeInfo s_aBccOpcodeInfo[NUMBER_OF_BCC_TYPES] =
{
    {0x17, Absolute, 1, 3}
};

static const OpcodeInfo s_aBcsOpcodeInfo[NUMBER_OF_BCS_TYPES] =
{
    {0x18, Absolute, 1, 3}
};

static const OpcodeInfo s_aBzcOpcodeInfo[NUMBER_OF_BZC_TYPES] =
{
    {0x1A, Absolute, 1, 3}
};

static const OpcodeInfo s_aBzsOpcodeInfo[NUMBER_OF_BZS_TYPES] =
{
    {0x1B, Absolute, 1, 3}
};

static const OpcodeInfo s_aClcOpcodeInfo[NUMBER_OF_CLC_TYPES] =
{
    {0x0B, Implied, 0, 1}
};

static const OpcodeInfo s_aDdpOpcodeInfo[NUMBER_OF_DDP_TYPES] =
{
    {0x09, Implied, 0, 1}
};

static const OpcodeInfo s_aIdpOpcodeInfo[NUMBER_OF_IDP_TYPES] =
{
    {0x0A, Implied, 0, 1}
};

static const OpcodeInfo s_aJmpOpcodeInfo[NUMBER_OF_JMP_TYPES] =
{
    {0x16, Absolute, 1, 3}
};

static const OpcodeInfo s_aJsrOpcodeInfo[NUMBER_OF_JSR_TYPES] =
{
    {0x1C, Absolute, 1, 3}
};

static const OpcodeInfo s_aLdaOpcodeInfo[NUMBER_OF_LDA_TYPES] =
{
    {0x15, Immediate, 1, 2},
    {0x07, DirectPage, 1, 2},
    {0x1E, DirectPageIndirect, 1, 3},
    {0x0E, Absolute, 1, 3}
};

static const OpcodeInfo s_aLdpOpcodeInfo[NUMBER_OF_LDP_TYPES] =
{
    {0x19, Immediate, 1, 2},
    {0x02, Absolute, 1, 3}
};

static const OpcodeInfo s_aOraOpcodeInfo[NUMBER_OF_ORA_TYPES] =
{
    {0x13, Immediate, 1, 2},
    {0x05, DirectPage, 1, 2}
};

static const OpcodeInfo s_aRolOpcodeInfo[NUMBER_OF_ROL_TYPES] =
{
    {0x03, Implied, 0, 1}
};

static const OpcodeInfo s_aRorOpcodeInfo[NUMBER_OF_ROR_TYPES] =
{
    {0x0D, Implied, 0, 1}
};

static const OpcodeInfo s_aRtsOpcodeInfo[NUMBER_OF_RTS_TYPES] =
{
    {0x1D, Implied, 0, 1}
};

static const OpcodeInfo s_aSecOpcodeInfo[NUMBER_OF_SEC_TYPES] =
{
    {0x0C, Implied, 0, 1}
};

static const OpcodeInfo s_aStaOpcodeInfo[NUMBER_OF_STA_TYPES] =
{
    {0x08, DirectPage, 1, 2},
    {0x1F, DirectPageIndirect, 1, 3},
    {0x0F, Absolute, 1, 3}
};

static const OpcodeInfo s_aSubOpcodeInfo[NUMBER_OF_SUB_TYPES] =
{
    {0x11, Immediate, 1, 2},
    {0x01, DirectPage, 1, 2}
};

static const OpcodeInfo s_aXorOpcodeInfo[NUMBER_OF_XOR_TYPES] =
{
    {0x14, Immediate, 1, 2},
    {0x06, DirectPage, 1, 2}
};

static const InstructionInfo s_aInstructionTable[] =
{
    {AddInstruction, ADD_INSTRUCTION_TEXT, NUMBER_OF_ADD_TYPES, s_aAddOpcodeInfo},
    {AndInstruction, AND_INSTRUCTION_TEXT, NUMBER_OF_AND_TYPES, s_aAndOpcodeInfo},
    {BccInstruction, BCC_INSTRUCTION_TEXT, NUMBER_OF_BCC_TYPES, s_aBccOpcodeInfo},
    {BcsInstruction, BCS_INSTRUCTION_TEXT, NUMBER_OF_BCS_TYPES, s_aBcsOpcodeInfo},
    {BzcInstruction, BZC_INSTRUCTION_TEXT, NUMBER_OF_BZC_TYPES, s_aBzcOpcodeInfo},
    {BzsInstruction, BZS_INSTRUCTION_TEXT, NUMBER_OF_BZS_TYPES, s_aBzsOpcodeInfo},
    {ClcInstruction, CLC_INSTRUCTION_TEXT, NUMBER_OF_CLC_TYPES, s_aClcOpcodeInfo},
    {DdpInstruction, DDP_INSTRUCTION_TEXT, NUMBER_OF_DDP_TYPES, s_aDdpOpcodeInfo},
    {IdpInstruction, IDP_INSTRUCTION_TEXT, NUMBER_OF_IDP_TYPES, s_aIdpOpcodeInfo},
    {JmpInstruction, JMP_INSTRUCTION_TEXT, NUMBER_OF_JMP_TYPES, s_aJmpOpcodeInfo},
    {JsrInstruction, JSR_INSTRUCTION_TEXT, NUMBER_OF_JSR_TYPES, s_aJsrOpcodeInfo},
    {LdaInstruction, LDA_INSTRUCTION_TEXT, NUMBER_OF_LDA_TYPES, s_aLdaOpcodeInfo},
    {LdpInstruction, LDP_INSTRUCTION_TEXT, NUMBER_OF_LDP_TYPES, s_aLdpOpcodeInfo},
    {OraInstruction, ORA_INSTRUCTION_TEXT, NUMBER_OF_ORA_TYPES, s_aOraOpcodeInfo},
    {RolInstruction, ROL_INSTRUCTION_TEXT, NUMBER_OF_ROL_TYPES, s_aRolOpcodeInfo},
    {RorInstruction, ROR_INSTRUCTION_TEXT, NUMBER_OF_ROR_TYPES, s_aRorOpcodeInfo},
    {RtsInstruction, RTS_INSTRUCTION_TEXT, NUMBER_OF_RTS_TYPES, s_aRtsOpcodeInfo},
    {SecInstruction, SEC_INSTRUCTION_TEXT, NUMBER_OF_SEC_TYPES, s_aSecOpcodeInfo},
    {StaInstruction, STA_INSTRUCTION_TEXT, NUMBER_OF_STA_TYPES, s_aStaOpcodeInfo},
    {SubInstruction, SUB_INSTRUCTION_TEXT, NUMBER_OF_SUB_TYPES, s_aSubOpcodeInfo},
    {XorInstruction, XOR_INSTRUCTION_TEXT, NUMBER_OF_XOR_TYPES, s_aXorOpcodeInfo}
};

//------------------------------------------------------------------------------
//Static Prototypes
static int parse_source_line(void);
static int lexical_scan_source_line(void);
static int label_lexer(void);
static int statement_lexer(void);
static int search_directive_table(const char* cpDirective);
static int search_instruction_table(const char* cpMnemonic);
static int do_byte_directive(char* cpCurrentStatementPosition);
static int do_end_directive(char* cpCurrentStatementPosition);
static int do_equ_directive(char* cpCurrentStatementPosition, char* cpSymbol);
static int do_org_directive(char* cpCurrentStatementPosition);
static void show_line_error(uint32_t iSourceLineErrorIndex);
static void print_symbol_to_table(BSTreeNode* pNode);

//==============================================================================
//Functions
/*------------------------------------------------------------------------------
 * Function name:  do_assembly
 * Function Description:  Performs a two-pass assembly on the source file.
 * Parameters:  None.
 * Returns:  Zero for success and negative number for failure.
------------------------------------------------------------------------------*/
int do_assembly(void)
{
    char caText[LISTING_FILE_MAX_COLUMNS + NULL_TERMINATING_BYTE_LENGTH];
    char caTimeString[30];

    int iReturnValue;
    int iFunctionReturnValue;

    uint32_t iVersionStringField;
    uint32_t iDateTimeField;
    uint32_t iCounter;
    uint32_t iFirstCodeByteLocation;

    ssize_t iReadSourceFileReturnValue;

    size_t iSourceLineBufferSize;
    size_t iFileNameLength;
    size_t iVersionStringLength;

    time_t currentTime;

    struct tm* pLocalTime;

    //Initialize variables.
    iSourceLineBufferSize = 0;

    //Clear out the lexer information structure.
    memset(&s_lexerInfo, 0, sizeof(s_lexerInfo));

    //Initialize program memory storage to -1.  When it comes time to write the
    //binary file we will be able to easily determine where the first byte of
    //code starts.
    for(iCounter = 0; iCounter < MAX_PROGRAM_MEMORY; iCounter++)
        s_lexerInfo.m_iaProgramMemory[iCounter] = -1;

    //Open the source file and check for success.
    iReturnValue = open_source_file();
    if(iReturnValue != EXIT_SUCCESS)
        return iReturnValue;

    //This is a two pass assembler.
    for(s_lexerInfo.m_iPass = PassOne; s_lexerInfo.m_iPass <= PassTwo; s_lexerInfo.m_iPass++)
    {
        //Before starting the pass reset all variables.
        s_lexerInfo.m_iSourceLineNumber = 0;
        s_lexerInfo.m_iLocationCounter = 0;

        //Reset the source file and check for success.
        iReturnValue = reset_source_file();
        if(iReturnValue != EXIT_SUCCESS)
            return iReturnValue;

        //If this is pass two then open the listing file if the listing table is
        //enabled.
        if(s_lexerInfo.m_iPass == PassTwo)
        {
            if(is_listing_file_enabled() == TRUE)
            {
                iReturnValue = open_listing_file();
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;

                //Listing file opened successfully so write the title.
                snprintf(caText, sizeof(caText), "%*s", LISTING_FILE_MAX_COLUMNS / 2 + ((sizeof(LISTING_FILE_TITLE) - NULL_TERMINATING_BYTE_LENGTH) / 2), LISTING_FILE_TITLE);
                iReturnValue = write_line_to_listing_file(caText);
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;

                //Write the information line which contains the file name,
                //program version, and date/time of assembly.  First get the
                //current date and time, convert to local time, and then format.
                currentTime = time(NULL);
                pLocalTime = localtime(&currentTime);
                strftime(caTimeString, sizeof(caTimeString), "%a %b %d %H:%M:%S %Y", pLocalTime);

                //Get the sizes of the necessary strings.
                iFileNameLength = strlen(get_assembly_source_full_file_name());
                iVersionStringLength = sizeof(VERSION) - NULL_TERMINATING_BYTE_LENGTH;

                //Calculate the field sizes.
                iVersionStringField = LISTING_FILE_MAX_COLUMNS / 2 + (iVersionStringLength / 2) - iFileNameLength;
                iDateTimeField = LISTING_FILE_MAX_COLUMNS - iFileNameLength - iVersionStringField;

                //Write the information to the listing file.
                snprintf(caText, sizeof(caText), "%s%*s%*s", get_assembly_source_full_file_name(), iVersionStringField, VERSION, iDateTimeField, caTimeString);
                iReturnValue = write_line_to_listing_file(caText);
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;

                //Write the header.
                snprintf(caText, sizeof(caText), "\r\n%-*s%-*s%-*s", LISTING_FILE_LINE_FIELD_LENGTH, "", LISTING_FILE_LC_FIELD_LENGTH, "", LISTING_FILE_OBJECT_CODE_FIELD_LENGTH, "OBJECT");
                iReturnValue = write_line_to_listing_file(caText);
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;

                snprintf(caText, sizeof(caText), "%-*s%-*s%-*s%s", LISTING_FILE_LINE_FIELD_LENGTH, "LINE#", LISTING_FILE_LC_FIELD_LENGTH, "LOC", LISTING_FILE_OBJECT_CODE_FIELD_LENGTH, "CODE", "LINE");
                iReturnValue = write_line_to_listing_file(caText);
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;

                memset(caText, '-', sizeof(caText));
                caText[LISTING_FILE_MAX_COLUMNS] = '\0';
                iReturnValue = write_line_to_listing_file(caText);
                if(iReturnValue != EXIT_SUCCESS)
                    return iReturnValue;
            }
        }

        //Loop through all the lines in the source file until either an error
        //occurs or the end of the file is reached.
        do
        {
            //Read a line from the source file.  Check the return value for the
            //next course of action.  Any value greater than zero indicates a
            //line was read, a negative value indicates an error, and a value of
            //zero means that the end of the file has been reached in which case
            //we will exit from the do-while() loop and do the next pass.
            s_lexerInfo.m_iSourceLineNumber++;
            iReadSourceFileReturnValue = read_line_from_source_file(&s_cpSourceLine, &iSourceLineBufferSize);
            if(iReadSourceFileReturnValue > 0)
            {
                //We have a line of text.  Save off the number of characters in
                //the line.
                s_lexerInfo.m_iSourceLineLength = (uint32_t)iReadSourceFileReturnValue;

                //This line needs can be parsed and lexical scanned.  Check the
                //return value.  If an error occurs then exit.  If an END
                //directive was found, which means we have reached the end of the
                //source file regardless of how many lines are left to read,
                //break out of the do/while() loop to start the next pass.
                iFunctionReturnValue = parse_source_line();
                if(iFunctionReturnValue < EXIT_SUCCESS)
                    return iFunctionReturnValue;
                else if(iFunctionReturnValue == END_DIRECTIVE_SUCCESS)
                    break;

                //The source line was parsed and lexical scan successfully
                //performed.  Check the location counter and check if it is over
                //the maximum allowed.
                if(s_lexerInfo.m_iLocationCounter > MAX_PROGRAM_MEMORY)
                {
                    print_error(__func__, ExceededProgramMemoryError);
                    show_line_error(s_lexerInfo.m_cpStatementMnemonicStart - s_cpSourceLine);
                    return -ExceededProgramMemoryError;
                }
            }
            else if(iReadSourceFileReturnValue < 0)
            {
                //An error occurred, return the error code.
                return (int)iReadSourceFileReturnValue;
            }
        } while(iReadSourceFileReturnValue > 0);

        printf("Pass %u completed successfully.\n", s_lexerInfo.m_iPass);
    }

    //If we are here both passes were successful.  Finish out the listing file by
    //adding the symbol table after the program listing if there are symbols,
    //the symbol table is enabled, and the listing file is enabled.
    if(s_lexerInfo.m_iLargestSymbolLength != 0 && is_symbol_table_enabled() == TRUE && is_listing_file_enabled() == TRUE)
    {
        //Set the field length to the largest symbol plus 2 for for white spaces
        //between the symbol and the value.  Check the largest symbol against the
        //word "SYMBOL" which is 6 characters.
        s_iStatementSymbolFieldLength = bmc_max(s_lexerInfo.m_iLargestSymbolLength, sizeof("SYMBOL") - NULL_TERMINATING_BYTE_LENGTH) + 2;

        //Write the header.
        snprintf(caText, sizeof(caText), "\r\n%-*s%s", s_iStatementSymbolFieldLength, "SYMBOL", "VALUE");
        iReturnValue = write_line_to_listing_file(caText);
        if(iReturnValue != EXIT_SUCCESS)
            return iReturnValue;

        memset(caText, '-', sizeof(caText));
        caText[LISTING_FILE_MAX_COLUMNS] = '\0';
        iReturnValue = write_line_to_listing_file(caText);
        if(iReturnValue != EXIT_SUCCESS)
            return iReturnValue;

        //Header written.  Print the symbol table in ascending order.
        bstree_in_order_walk(s_pSymbolTableRoot, print_symbol_to_table);
    }

    //Open the binary file and check for success.
    iReturnValue = open_binary_file();
    if(iReturnValue != EXIT_SUCCESS)
        return iReturnValue;

    //Binary file opened successfully.  Find the first byte of code in the
    //program memory.
    for(iFirstCodeByteLocation = 0; iFirstCodeByteLocation < MAX_PROGRAM_MEMORY; iFirstCodeByteLocation++)
    {
        if(s_lexerInfo.m_iaProgramMemory[iFirstCodeByteLocation] != -1)
            break;
    }

    //If we didn't make it all the way to the end of the program memory then
    //there is binary data to write.
    if(iFirstCodeByteLocation < MAX_PROGRAM_MEMORY)
    {
        //Write all the data in program memory.
        iReturnValue = write_data_to_binary_file(&s_lexerInfo.m_iaProgramMemory[iFirstCodeByteLocation], s_lexerInfo.m_iLocationCounter - iFirstCodeByteLocation);
        if(iReturnValue != EXIT_SUCCESS)
            return iReturnValue;
    }

    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  free_lexer_memory
 * Function Description:  Frees all allocated memory used by the parser.
 * Parameters:  None.
 * Returns:  None.
------------------------------------------------------------------------------*/
void free_lexer_memory(void)
{
    if(s_cpSourceLine != NULL)
        free(s_cpSourceLine);

    while(s_pSymbolTableRoot != NULL)
        bstree_delete(&s_pSymbolTableRoot, s_pSymbolTableRoot);
}

/*------------------------------------------------------------------------------
 * Function name:  get_location_counter_value
 * Function Description:  Getter function for the location counter.
 * Parameters:  None.
 * Returns:  The current value of the location counter.
------------------------------------------------------------------------------*/
uint32_t get_location_counter_value(void)
{
    return s_lexerInfo.m_iLocationCounter;
}

/*------------------------------------------------------------------------------
 * Function name:  get_symbol_table_root
 * Function Description:  Getter function for the symbol table root.
 * Parameters:  None.
 * Returns:  A pointer to the symbol table root.
------------------------------------------------------------------------------*/
BSTreeNode* get_symbol_table_root(void)
{
    return s_pSymbolTableRoot;
}

/*------------------------------------------------------------------------------
 * Function name:  parse_source_line
 * Function Description:  Parses a source file line.
 * Parameters:  None
 * Returns:  Zero for success and continue, END_DIRECTIVE_SUCCESS for success and
 *           END directive was reached, and a negative number if an error occurs.
------------------------------------------------------------------------------*/
static int parse_source_line(void)
{
    uint32_t iIndex;

    //Initialize variables.
    s_lexerInfo.m_cpStartOfLabel = NULL;
    s_lexerInfo.m_cpEndOfComment = NULL;

    //First we want a common letter case.  Convert all letters to upper case with
    //the exception of:
    //1.  First character after a single quote
    //2.  All characters within double quotes
    //Continue converting to either the first ';' character (not after a single
    //quote and not within double quotes) or EOL.
    iIndex = 0;
    while(s_cpSourceLine[iIndex] != ';' && iIndex < s_lexerInfo.m_iSourceLineLength)
    {
        if(s_cpSourceLine[iIndex] == '\'')
        {
            //We reached a single quote.  Skip it and the character immediately
            //following.
            iIndex += 2;
        }
        else if(s_cpSourceLine[iIndex] == '"')
        {
            //We reached a double quote, skip over it.
            iIndex++;

            //Loop until the next double quote, or EOL, is reached.
            while(s_cpSourceLine[iIndex] != '"' && iIndex < s_lexerInfo.m_iSourceLineLength)
            {
                //There can be escaped characters in a string and those need to
                //be converted to upper case.  These characters are preceded by
                //a backslash character.
                if(s_cpSourceLine[iIndex] == '\\')
                {
                    //Backslash character found.  Skip past it.
                    iIndex++;

                    //Convert the following character and then skip past.
                    s_cpSourceLine[iIndex] = toupper(s_cpSourceLine[iIndex]);
                    iIndex++;
                }
                else
                {
                    //Not a backslash so skip past the character.
                    iIndex++;
                }
            }

            //If we are pointing at a double quote then skip over it.
            if(s_cpSourceLine[iIndex] == '"')
                iIndex++;
        }
        else
        {
            //Neither a single quote or double quote so do conversion.
            s_cpSourceLine[iIndex] = toupper(s_cpSourceLine[iIndex]);
            iIndex++;
        }
    }

    //Convert tabs to spaces up to the EOL.
    for(iIndex = 0; iIndex < s_lexerInfo.m_iSourceLineLength; iIndex++)
    {
        if(s_cpSourceLine[iIndex] == '\t')
            s_cpSourceLine[iIndex] = ' ';
    }

    //A line is defined as:
    //<line> ::= [<label>] [<statement>] [<comment>] <eol>
    //First lets check for an optional comment by looking for the first
    //occurrence of a ';' character that does not immediately follow a single
    //quote or is not within double quotes.
    iIndex = 0;
    while(iIndex < s_lexerInfo.m_iSourceLineLength)
    {
        if(s_cpSourceLine[iIndex] == '\'')
        {
            //We reached a single quote.  Skip it and the character immediately
            //following.
            iIndex += 2;
        }
        else if(s_cpSourceLine[iIndex] == '"')
        {
            //We reached a double quote, skip over it.
            iIndex++;

            //Loop until the next double quote, or EOL, is reached.
            while(s_cpSourceLine[iIndex] != '"' && iIndex < s_lexerInfo.m_iSourceLineLength)
                    iIndex++;

            //If we are pointing at a double quote then skip over it.
            if(s_cpSourceLine[iIndex] == '"')
                iIndex++;
        }
        else
        {
            //Neither a single quote or double quote.  If it is a ';' character
            //than break from the while loop.  Otherwise skip past the character.
            if(s_cpSourceLine[iIndex] == ';')
                break;

            iIndex++;
        }
    }

    //We are out of the while() loop.  If we didn't make it to EOL then we found
    //a comment.
    s_lexerInfo.m_cpStartOfComment = (iIndex < s_lexerInfo.m_iSourceLineLength) ? s_cpSourceLine + iIndex : NULL;
    if(s_lexerInfo.m_cpStartOfComment != NULL)
        s_lexerInfo.m_cpEndOfComment = s_cpSourceLine + s_lexerInfo.m_iSourceLineLength - 1;

    //Check if we have a comment that starts at the beginning of the line.
    if(s_lexerInfo.m_cpStartOfComment != s_cpSourceLine)
    {
        //We do not have a comment that starts at the beginning of the line so
        //there is more line to parse.  Next let's check for an optional label by
        //looking for the first occurrence of a ':' character that does not
        //immediately follow a single quote or is not within double quotes.
        iIndex = 0;
        while(iIndex < s_lexerInfo.m_iSourceLineLength)
        {
            if(s_cpSourceLine[iIndex] == '\'')
            {
                //We reached a single quote.  Skip it and the character
                //immediately following.
                iIndex += 2;
            }
            else if(s_cpSourceLine[iIndex] == '"')
            {
                //We reached a double quote, skip over it.
                iIndex++;

                //Loop until the next double quote, or EOL, is reached.
                while(s_cpSourceLine[iIndex] != '"' && iIndex < s_lexerInfo.m_iSourceLineLength)
                        iIndex++;

                //If we are pointing at a double quote then skip over it.
                if(s_cpSourceLine[iIndex] == '"')
                    iIndex++;
            }
            else
            {
                //Neither a single quote or double quote.  If it is a ':'
                //character than break from the while loop.  Otherwise skip past
                //the character.
                if(s_cpSourceLine[iIndex] == ':')
                    break;

                iIndex++;
            }
        }

        //We are out of the while() loop.  If we didn't make it to EOL then we found
        //a label.
        s_lexerInfo.m_cpEndOfLabel = (iIndex < s_lexerInfo.m_iSourceLineLength) ? s_cpSourceLine + iIndex : NULL;
        if(s_lexerInfo.m_cpEndOfLabel != NULL)
        {
            //A ':' character was found.  If the ':' is within the comment then
            //it isn't a label.
            if(s_lexerInfo.m_cpStartOfComment != NULL && s_lexerInfo.m_cpEndOfLabel > s_lexerInfo.m_cpStartOfComment)
            {
                //The ':' is within the comment so there is no label.
                s_lexerInfo.m_cpEndOfLabel = NULL;
            }
            else
            {
                //The ':' is not within the comment.  The start of the label will
                //be the start of the line.
                s_lexerInfo.m_cpStartOfLabel = s_cpSourceLine;
            }
        }

        //Everything between the optional label and optional comment is the
        //optional statement.
        //1.  If no label and no comment then the start and stop of the statement
        //    is the entire line.
        //2.  If there is a label and no comment then the statement starts just
        //    after the label to the end of the line.
        //3.  If no label and a comment then the statement starts at the
        //    beginning of the line to the start of the comment.
        //4.  If there is a label and there is a comment then the statement
        //    starts just after the label and ends just before the comment.
        if(s_lexerInfo.m_cpStartOfLabel == NULL && s_lexerInfo.m_cpStartOfComment == NULL)
        {
            s_lexerInfo.m_cpStartOfStatement = s_cpSourceLine;
            s_lexerInfo.m_cpEndOfStatement = s_cpSourceLine + s_lexerInfo.m_iSourceLineLength - 1;
        }
        else if(s_lexerInfo.m_cpStartOfLabel != NULL && s_lexerInfo.m_cpStartOfComment == NULL)
        {
            s_lexerInfo.m_cpStartOfStatement = s_lexerInfo.m_cpEndOfLabel + 1;
            s_lexerInfo.m_cpEndOfStatement = s_cpSourceLine + s_lexerInfo.m_iSourceLineLength - 1;
        }
        else if(s_lexerInfo.m_cpStartOfLabel == NULL && s_lexerInfo.m_cpStartOfComment != NULL)
        {
            s_lexerInfo.m_cpStartOfStatement = s_cpSourceLine;
            s_lexerInfo.m_cpEndOfStatement = s_lexerInfo.m_cpStartOfComment - 1;
        }
        else
        {
            s_lexerInfo.m_cpStartOfStatement = s_lexerInfo.m_cpEndOfLabel + 1;
            s_lexerInfo.m_cpEndOfStatement = s_lexerInfo.m_cpStartOfComment - 1;
        }
    }
    else
    {
        //The comment starts at the beginning of the line so there is no more
        //line to parse.
        s_lexerInfo.m_cpEndOfLabel = NULL;
        s_lexerInfo.m_cpStartOfStatement = NULL;
        s_lexerInfo.m_cpEndOfStatement = NULL;
    }

    //At this point the line has been broken into optional label, statement, and
    //comment sections.  Eliminate leading white spaces of the label section.
    if(s_lexerInfo.m_cpStartOfLabel != NULL)
    {
        //Loop until the end of the label section is reached or a non-whitespace
        //character has been found.
        while(s_lexerInfo.m_cpStartOfLabel <= s_lexerInfo.m_cpEndOfLabel && isspace(*s_lexerInfo.m_cpStartOfLabel) != 0)
            s_lexerInfo.m_cpStartOfLabel++;

        //We are out of the while() loop.  If the start of the section overcame
        //the end of the section then the section contained all white spaces.
        if(s_lexerInfo.m_cpStartOfLabel <= s_lexerInfo.m_cpEndOfLabel)
        {
            //Check if the label exceeds the maximum size.
            if((s_lexerInfo.m_cpEndOfLabel - s_lexerInfo.m_cpStartOfLabel) > MAX_SYMBOL_SIZE)
            {
                print_error(__func__, SymbolLengthExceedsMaximumError);
                show_line_error(s_lexerInfo.m_cpStartOfLabel - s_cpSourceLine);
                return -SymbolLengthExceedsMaximumError;
            }
        }
        else
        {
            //The label was all white spaces.
            s_lexerInfo.m_cpStartOfLabel = NULL;
            s_lexerInfo.m_cpEndOfLabel = NULL;
        }
    }

    //Eliminate leading and trailing white spaces of the statement section.
    if(s_lexerInfo.m_cpStartOfStatement != NULL)
    {
        //First do leading white spaces.  Loop until the end of the statement
        //section is reached or a non- whitespace character has been found.
        while(s_lexerInfo.m_cpStartOfStatement <= s_lexerInfo.m_cpEndOfStatement && isspace(*s_lexerInfo.m_cpStartOfStatement) != 0)
            s_lexerInfo.m_cpStartOfStatement++;

        //We are out of the while() loop.  Check if we still have a statement
        //section to check for trailing white spaces.
        if(s_lexerInfo.m_cpStartOfStatement <= s_lexerInfo.m_cpEndOfStatement)
        {
            //We are still in the statement section so we can check for trailing
            //white spaces.
            while(isspace(*s_lexerInfo.m_cpEndOfStatement) != 0)
                s_lexerInfo.m_cpEndOfStatement--;
        }
        else
        {
            //The statement section contained all white spaces.
            s_lexerInfo.m_cpStartOfStatement = NULL;
            s_lexerInfo.m_cpEndOfStatement = NULL;
        }
    }

    //Eliminate trailing white spaces of the comment section.
    if(s_lexerInfo.m_cpStartOfComment != NULL)
    {
        while(isspace(*s_lexerInfo.m_cpEndOfComment) != 0)
            s_lexerInfo.m_cpEndOfComment--;
    }

    //The line has been parsed so now we can perform a lexical scan.
    return lexical_scan_source_line();
}

/*------------------------------------------------------------------------------
 * Function name:  lexical_scan_source_line
 * Function Description:  Performs a lexical scan of a parsed source file line.
 * Parameters:  None
 * Returns:  Zero for success and continue, END_DIRECTIVE_SUCCESS for success and
 *           END directive was reached, and a negative number if an error occurs.
------------------------------------------------------------------------------*/
static int lexical_scan_source_line(void)
{
    char caListing[LISTING_FILE_MAX_COLUMNS + NULL_TERMINATING_BYTE_LENGTH];
    char caLineNumber[LISTING_FILE_LINE_MAX_DIGITS + NULL_TERMINATING_BYTE_LENGTH];
    char caLocationCounter[LISTING_FILE_LC_MAX_CHARACTERS + NULL_TERMINATING_BYTE_LENGTH];
    char caObjectCodeByte[LISTING_FILE_OBJECT_CODE_BYTE_MAX_CHARACTERS + NULL_TERMINATING_BYTE_LENGTH];
    char caObjectCode[LISTING_FILE_OBJECT_CODE_MAX_CHARACTERS + NULL_TERMINATING_BYTE_LENGTH];
    char caSource[LISTING_FILE_MAX_COLUMNS + NULL_TERMINATING_BYTE_LENGTH];

    uint8_t bWithinDoubleQuote;

    int iLexerScanReturnValue;
    int iFunctionReturnValue;

    uint32_t iPreviousLocationCounter;
    uint32_t iStatementSymbolFieldLength;
    uint32_t iStatementMnemonicFieldLength;
    uint32_t iCounter;

    size_t iLength;

    //Assume success and continue.
    iLexerScanReturnValue = EXIT_SUCCESS;

    //Initialize variables.
    s_lexerInfo.m_cpStatementSymbolStart = NULL;
    s_lexerInfo.m_cpStatementSymbolEnd = NULL;
    s_lexerInfo.m_cpStatementMnemonicStart = NULL;
    s_lexerInfo.m_cpStatementMnemonicEnd = NULL;
    s_lexerInfo.m_cpStatementExpresionStart = NULL;

    //Save off the current location counter.
    iPreviousLocationCounter = s_lexerInfo.m_iLocationCounter;

    //If the line has a label section then it needs to be evaluated.
    if(s_lexerInfo.m_cpStartOfLabel != NULL)
    {
        iLexerScanReturnValue = label_lexer();
        if(iLexerScanReturnValue != EXIT_SUCCESS)
            return iLexerScanReturnValue;
    }

    //If there is a statement section then it needs to be evaluated.
    if(s_lexerInfo.m_cpStartOfStatement != NULL)
    {
        iLexerScanReturnValue = statement_lexer();
        if(iLexerScanReturnValue < EXIT_SUCCESS)
            return iLexerScanReturnValue;

        //Check if this is pass one and there are operands.
        if(s_lexerInfo.m_iPass == PassOne && s_lexerInfo.m_cpStatementExpresionStart != NULL)
        {
            //We need to count the number of characters in the operand field,
            //excluding white spaces that are not within double quotes.
            bWithinDoubleQuote = FALSE;
            iCounter = 0;
            while(s_lexerInfo.m_cpStatementExpresionStart <= s_lexerInfo.m_cpEndOfStatement)
            {
                if(bWithinDoubleQuote == FALSE)
                {
                    if(isspace(*s_lexerInfo.m_cpStatementExpresionStart) == 0)
                        iCounter++;

                    if(*s_lexerInfo.m_cpStatementExpresionStart == '"' && *(s_lexerInfo.m_cpStatementExpresionStart - 1) != '\\')
                        bWithinDoubleQuote = TRUE;
                }
                else
                {
                    iCounter++;

                    if(*s_lexerInfo.m_cpStatementExpresionStart == '"' && *(s_lexerInfo.m_cpStatementExpresionStart - 1) != '\\')
                        bWithinDoubleQuote = FALSE;
                }

                (s_lexerInfo.m_cpStatementExpresionStart)++;
            }

            //Add one more for a white space after the operands.
            iCounter++;

            //Check if this is the largest operand field so far.
            s_lexerInfo.m_iLargestOperandLength = bmc_max(iCounter, s_lexerInfo.m_iLargestOperandLength);
        }
    }

    //If this is pass two write the line to the listing file.
    if(s_lexerInfo.m_iPass == PassTwo)
    {
        //Create the line number field.
        snprintf(caLineNumber, sizeof(caLineNumber), "%0*u", LISTING_FILE_LINE_MAX_DIGITS, s_lexerInfo.m_iSourceLineNumber);

        //Create the location counter field.
        snprintf(caLocationCounter, sizeof(caLocationCounter), "%0*X", LISTING_FILE_LC_MAX_CHARACTERS, iPreviousLocationCounter);

        //If there is object code then fill in the object code field.  There is
        //one exception to this and that is the ORG directive.  The ORG directive
        //will change the location counter but does not produce any object code.
        caObjectCode[0] = '\0';
        if(s_lexerInfo.m_iLocationCounter != iPreviousLocationCounter && iLexerScanReturnValue != ORG_DIRECTIVE_SUCCESS)
        {
            //The location counter changed and it wasn't the ORG directive so we
            //have object code.  Up to three bytes can fit in the object code
            //field.
            for(iCounter = 0; iCounter < LISTING_FILE_OBJECT_CODE_BYTES_PER_FIELD && s_lexerInfo.m_iLocationCounter != iPreviousLocationCounter; iCounter++)
            {
                snprintf(caObjectCodeByte, sizeof(caObjectCodeByte), "%02X ", (uint8_t)(s_lexerInfo.m_iaProgramMemory[iPreviousLocationCounter]));
                strcat(caObjectCode, caObjectCodeByte);
                iPreviousLocationCounter++;
            }
        }

        //Create the source field.
        caSource[0] = '\0';
        if(s_lexerInfo.m_cpStartOfLabel != NULL || s_lexerInfo.m_cpStartOfStatement != NULL || s_lexerInfo.m_cpStartOfComment != NULL)
        {
            //There is something to put in the source field.  Check for comment
            //only.
            if(s_lexerInfo.m_cpStartOfLabel == NULL && s_lexerInfo.m_cpStartOfStatement == NULL && s_lexerInfo.m_cpStartOfComment != NULL)
            {
                //The source file only contains a comment.  Start it at the
                //beginning of the field.  Put a NULL terminator after the
                //comment and copy the whole comment.
                *(s_lexerInfo.m_cpEndOfComment + 1) = '\0';
                snprintf(caSource, sizeof(caSource), "%s", s_lexerInfo.m_cpStartOfComment);
            }
            else
            {
                //There is more to the line then just a comment.  If we have
                //symbols then set the field length to the size of the largest
                //symbol and add two for the ':' character after labels and a
                //whitespace.
                iStatementSymbolFieldLength = (s_lexerInfo.m_iLargestSymbolLength == 0) ? 0 : s_lexerInfo.m_iLargestSymbolLength + 2;

                //The statement mnemonic field length is the length of the
                //largest mnemonic plus one for a whitespace.
                iStatementMnemonicFieldLength = MAX_DIR_MNEMONIC_SIZE + 1;

                //Place NULL terminators after the symbol and mnemonic if they
                //exist on the line.
                if(s_lexerInfo.m_cpStatementSymbolStart != NULL)
                    *(s_lexerInfo.m_cpStatementSymbolEnd + 1) = '\0';
                if(s_lexerInfo.m_cpStatementMnemonicStart != NULL)
                    *(s_lexerInfo.m_cpStatementMnemonicEnd + 1) = '\0';

                //Assemble the source line with the symbol and mnemonic.
                snprintf(caSource, sizeof(caSource), "%-*s%-*s", iStatementSymbolFieldLength, (s_lexerInfo.m_cpStatementSymbolStart == NULL) ? "" : s_lexerInfo.m_cpStatementSymbolStart, iStatementMnemonicFieldLength, (s_lexerInfo.m_cpStatementMnemonicStart == NULL) ? "" : s_lexerInfo.m_cpStatementMnemonicStart);

                //Check if there are operands.
                if(s_lexerInfo.m_cpStatementExpresionStart != NULL)
                {
                    //Add all operand text without white spaces, except those
                    //within double quotes.  Loop until the operand field has
                    //been filled or we run out of space in the string.
                    bWithinDoubleQuote = FALSE;
                    iCounter = 0;
                    iLength = strlen(caSource);
                    while(iCounter < s_lexerInfo.m_iLargestOperandLength && (iLength + iCounter) < (sizeof(caSource) - NULL_TERMINATING_BYTE_LENGTH))
                    {
                        //We have filled the entire operand field and there is
                        //still space in the string.  Check if we have more
                        //operand characters.
                        if(s_lexerInfo.m_cpStatementExpresionStart <= s_lexerInfo.m_cpEndOfStatement)
                        {
                            //There are more operand characters.  Don't add white
                            //spaces unless they are in double quotes.
                            if(bWithinDoubleQuote == FALSE)
                            {
                                if(isspace(*s_lexerInfo.m_cpStatementExpresionStart) == 0)
                                {
                                    strncat(caSource, s_lexerInfo.m_cpStatementExpresionStart, 1);
                                    iCounter++;
                                }

                                if(*s_lexerInfo.m_cpStatementExpresionStart == '"' && *(s_lexerInfo.m_cpStatementExpresionStart - 1) != '\\')
                                    bWithinDoubleQuote = TRUE;
                            }
                            else
                            {
                                strncat(caSource, s_lexerInfo.m_cpStatementExpresionStart, 1);
                                iCounter++;

                                if(*s_lexerInfo.m_cpStatementExpresionStart == '"' && *(s_lexerInfo.m_cpStatementExpresionStart - 1) != '\\')
                                    bWithinDoubleQuote = FALSE;
                            }

                            (s_lexerInfo.m_cpStatementExpresionStart)++;
                        }
                        else
                        {
                            //No more operand characters left so fill the field
                            //with white spaces.
                            strncat(caSource, " ", 1);
                            iCounter++;
                        }
                    }
                }
                else if(s_lexerInfo.m_cpStatementExpresionStart == NULL && s_lexerInfo.m_cpStartOfComment != NULL)
                {
                    //There are no operands but there is a comment on the line.
                    //We need to fill the operand field with spaces to align the
                    //comment text at the start of the comment field.  Loop until
                    //all the field has been filled or we run out of room in the
                    //string.
                    iCounter = 0;
                    iLength = strlen(caSource);
                    while(iCounter < s_lexerInfo.m_iLargestOperandLength && (iLength + iCounter) < (sizeof(caSource) - NULL_TERMINATING_BYTE_LENGTH))
                    {
                        strncat(caSource, " ", 1);
                        iCounter++;
                    }
                }

                //Check if there is a comment at the end of the line.
                if(s_lexerInfo.m_cpStartOfComment != NULL)
                {
                    //Calculate how much of it will fit in the remaining space of
                    //the string.
                    iCounter = bmc_min((sizeof(caSource) - NULL_TERMINATING_BYTE_LENGTH) - strlen(caSource), (size_t)(s_lexerInfo.m_cpEndOfComment - s_lexerInfo.m_cpStartOfComment + 1));
                    strncat(caSource, s_lexerInfo.m_cpStartOfComment, iCounter);
                }
            }
        }

        //Assemble the listing line.
        snprintf(caListing, sizeof(caListing), "%-*s%-*s%-*s%s", LISTING_FILE_LINE_FIELD_LENGTH, caLineNumber, LISTING_FILE_LC_FIELD_LENGTH, caLocationCounter, LISTING_FILE_OBJECT_CODE_FIELD_LENGTH, caObjectCode, caSource);

        //Remove any unneeded trailing white spaces from the listing line.
        iLength = strlen(caListing);
        iLength--;
        while(isspace(caListing[iLength]) != 0)
        {
            caListing[iLength] = '\0';
            iLength--;
        }

        //Print the listing line to the listing file if the listing table is
        //enabled.
        if(is_listing_file_enabled() == TRUE)
        {
            iFunctionReturnValue = write_line_to_listing_file(caListing);
            if(iFunctionReturnValue != EXIT_SUCCESS)
                return iFunctionReturnValue;
        }

        //Check if there is still more object code.  This can happen with the
        //BYTE directive and a list of numbers greater than four.
        if(s_lexerInfo.m_iLocationCounter != iPreviousLocationCounter && iLexerScanReturnValue != ORG_DIRECTIVE_SUCCESS)
        {
            while(s_lexerInfo.m_iLocationCounter != iPreviousLocationCounter)
            {
                //Create the location counter field.
                snprintf(caLocationCounter, sizeof(caLocationCounter), "%0*X", LISTING_FILE_LC_MAX_CHARACTERS, iPreviousLocationCounter);

                //Up to three bytes can fit in the object code field.
                caObjectCode[0] = '\0';
                for(iCounter = 0; iCounter < LISTING_FILE_OBJECT_CODE_BYTES_PER_FIELD && s_lexerInfo.m_iLocationCounter != iPreviousLocationCounter; iCounter++)
                {
                    snprintf(caObjectCodeByte, sizeof(caObjectCodeByte), "%02X ", (uint8_t)(s_lexerInfo.m_iaProgramMemory[iPreviousLocationCounter]));
                    strcat(caObjectCode, caObjectCodeByte);
                    iPreviousLocationCounter++;
                }

                //Assemble the listing line with only the line, location counter, and
                //object code fields.
                snprintf(caListing, sizeof(caListing), "%-*s%-*s%-*s", LISTING_FILE_LINE_FIELD_LENGTH, caLineNumber, LISTING_FILE_LC_FIELD_LENGTH, caLocationCounter, LISTING_FILE_OBJECT_CODE_FIELD_LENGTH, caObjectCode);

                //Remove any unneeded trailing white spaces from the listing line.
                iLength = strlen(caListing);
                iLength--;
                while(isspace(caListing[iLength]) != 0)
                {
                    caListing[iLength] = '\0';
                    iLength--;
                }

                //Print the listing line to the listing file if the listing table
                //is enabled.
                if(is_listing_file_enabled() == TRUE)
                {
                    iFunctionReturnValue = write_line_to_listing_file(caListing);
                    if(iFunctionReturnValue != EXIT_SUCCESS)
                        return iFunctionReturnValue;
                }
            }
        }
    }

    return (iLexerScanReturnValue == END_DIRECTIVE_SUCCESS) ? END_DIRECTIVE_SUCCESS : EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  label_lexer
 * Function Description:  Performs a lexical scan of the label portion of the
 *                        source file line.
 * Parameters:  None.
 * Returns:  Zero for success and a negative number if an error occurs.
------------------------------------------------------------------------------*/
static int label_lexer(void)
{
    char caSymbol[MAX_SYMBOL_SIZE + NULL_TERMINATING_BYTE_LENGTH];
    char* cpSourceLineCurrentPosition;

    int iFunctionReturnValue;

    BSTreeNode* pNode;

    //The start pointer points to the first character of the label.
    cpSourceLineCurrentPosition = s_lexerInfo.m_cpStartOfLabel;
    iFunctionReturnValue = get_symbol(&cpSourceLineCurrentPosition, caSymbol);
    if(iFunctionReturnValue != EXIT_SUCCESS)
    {
        print_error(__func__, (uint8_t)(-iFunctionReturnValue));
        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
        return iFunctionReturnValue;
    }

    //We should be pointing at the ':' character.
    if(*cpSourceLineCurrentPosition != ':')
    {
        //Invalid character in label.
        print_error(__func__, InvalidCharacterSyntaxError);
        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
        return -InvalidCharacterSyntaxError;
    }

    //If we are here then the label syntax is valid.  We only care about storing
    //it in the symbol table during pass one.
    if(s_lexerInfo.m_iPass == PassOne)
    {
        //This is pass one.  Check for duplicate symbol.
        pNode = bstree_search(s_pSymbolTableRoot, caSymbol, bstree_key_compare);
        if(pNode != NULL)
        {
            //The label is a duplicate.  Print the error and return the proper
            //error value.
            print_error(__func__, DuplicateSymbolError);
            show_line_error(s_lexerInfo.m_cpStartOfLabel - s_cpSourceLine);
            return -DuplicateSymbolError;
        }

        //The label is unique so it can be inserted into the symbol tree.
        pNode = bstree_insert(&s_pSymbolTableRoot, caSymbol, strlen(caSymbol) + NULL_TERMINATING_BYTE_LENGTH, bstree_key_compare);
        if(pNode == NULL)
        {
            print_error(__func__, MallocReturnedNull);
            return -MallocReturnedNull;
        }

        //Insert was successful.  We need to add the value of this label.  The
        //node includes a void pointer to a data element which can be anything.
        //We will use it an unsigned int pointer.
        pNode->m_vpDataElement = malloc(sizeof(uint32_t));
        if(pNode->m_vpDataElement == NULL)
        {
            print_error(__func__, MallocReturnedNull);
            return -MallocReturnedNull;
        }

        //Memory allocated successfully.  Store the current location counter as
        //the value.
        *(uint32_t*)(pNode->m_vpDataElement) = s_lexerInfo.m_iLocationCounter;

        //Check if the length of this symbol is larger then the current longest
        //symbol.
        s_lexerInfo.m_iLargestSymbolLength = bmc_max(strlen(caSymbol), s_lexerInfo.m_iLargestSymbolLength);
    }

    //Mark the start and end of this symbol.
    s_lexerInfo.m_cpStatementSymbolStart = s_lexerInfo.m_cpStartOfLabel;
    s_lexerInfo.m_cpStatementSymbolEnd = cpSourceLineCurrentPosition;

    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  statement_lexer
 * Function Description:  Performs a lexical scan of the statement portion of the
 *                        source file line.
 * Parameters:  None.
 * Returns:  A positive value for a successful directive, zero for a successful
 *           instruction, and a negative number if an error occurs.
------------------------------------------------------------------------------*/
static int statement_lexer(void)
{
    char caText[MAX_SYMBOL_SIZE + NULL_TERMINATING_BYTE_LENGTH];
    char* cpSourceLineCurrentPosition;
    char* cpTemp;

    uint8_t bFoundType;
    uint8_t iProvidedOperands;
    uint8_t iState;
    uint8_t iType;

    uint32_t iInstructionIndex;
    uint32_t iOpcodeInfoIndex;

    ExpressionInfo aOperands[MAX_OPERANDS];

    int iFunctionReturnValue;

    //The start pointer points to the first character of the statement and the
    //end pointer points to the last character of the statement.  A statement is
    //defined as:
    //<statement> ::= <directive> | <instruction>
    //A directive can start with either a symbol or directive name and an
    //instruction begins with a mnemonic.  We need to get the length first.  Loop
    //until either the first whitespace or the end of the statement is reached.
    for(cpSourceLineCurrentPosition = s_lexerInfo.m_cpStartOfStatement; isspace(*cpSourceLineCurrentPosition) == 0 && cpSourceLineCurrentPosition <= s_lexerInfo.m_cpEndOfStatement; cpSourceLineCurrentPosition++)
        ;

    //We are out of the for() loop.  If the size is greater than the maximum size
    //of a symbol, which is larger than either a directive name or mnemonic, this
    //is an error.
    if((cpSourceLineCurrentPosition - s_lexerInfo.m_cpStartOfStatement) > MAX_SYMBOL_SIZE)
    {
        print_error(__func__, SymbolLengthExceedsMaximumError);
        show_line_error(s_lexerInfo.m_cpStartOfLabel - s_cpSourceLine);
        return -SymbolLengthExceedsMaximumError;
    }

    //The size is correct for a symbol, directive name, or mnemonic.  Since
    //directive names and mnemonics must start with a letter and contain only
    //letters and numbers we can use the get_symbol() function.
    cpSourceLineCurrentPosition = s_lexerInfo.m_cpStartOfStatement;
    iFunctionReturnValue = get_symbol(&cpSourceLineCurrentPosition, caText);
    if(iFunctionReturnValue != EXIT_SUCCESS)
    {
        print_error(__func__, (uint8_t)(-iFunctionReturnValue));
        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
        return iFunctionReturnValue;
    }

    //We should either be pointing at a whitespace or the end of the statement
    //section.
    if(*cpSourceLineCurrentPosition != ' ' && cpSourceLineCurrentPosition <= s_lexerInfo.m_cpEndOfStatement)
    {
        print_error(__func__, InvalidCharacterSyntaxError);
        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
        return -InvalidCharacterSyntaxError;
    }

    //Do work based on if it is a directive name, mnemonics, or symbol.
    if((iFunctionReturnValue = search_directive_table(caText)) != -1)
    {
        //It is a directive.  Do work based on the directive  Mark the start and
        //end of this directive mnemonic.
        s_lexerInfo.m_cpStatementMnemonicStart = s_lexerInfo.m_cpStartOfStatement;
        s_lexerInfo.m_cpStatementMnemonicEnd = cpSourceLineCurrentPosition - 1;

        //Do work based on the directive.
        switch(s_aDirectiveTable[(uint32_t)iFunctionReturnValue].m_iIndex)
        {
            case ByteDirective :
                iFunctionReturnValue = do_byte_directive(cpSourceLineCurrentPosition);
                return (iFunctionReturnValue == EXIT_SUCCESS) ? BYTE_DIRECTIVE_SUCCESS : iFunctionReturnValue;
                break;
            case EndDirective :
                iFunctionReturnValue = do_end_directive(cpSourceLineCurrentPosition);
                return (iFunctionReturnValue == EXIT_SUCCESS) ? END_DIRECTIVE_SUCCESS : iFunctionReturnValue;
                break;
            case OrgDirective :
                iFunctionReturnValue = do_org_directive(cpSourceLineCurrentPosition);
                return (iFunctionReturnValue == EXIT_SUCCESS) ? ORG_DIRECTIVE_SUCCESS : iFunctionReturnValue;
                break;
            default :
                //Should never get here but put in for good programming practice.
                break;
        }
    }
    else if((iFunctionReturnValue = search_instruction_table(caText)) != -1)
    {
        //It is a mnemonic.  Save off the index into the instruction table.
        iInstructionIndex = (uint32_t)iFunctionReturnValue;

        //Mark the start and end of this instruction mnemonic.
        s_lexerInfo.m_cpStatementMnemonicStart = s_lexerInfo.m_cpStartOfStatement;
        s_lexerInfo.m_cpStatementMnemonicEnd = cpSourceLineCurrentPosition - 1;

        //Check if this instruction has operands.  If an instruction doesn't
        //have operands then it will only have one type (implied) so start by
        //assuming implied.
        iType = Implied;
        if(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[0].m_iNumberOfOperands == 0)
        {
            //There are no operands.  We should be past the end of the statement.
            if(cpSourceLineCurrentPosition <= s_lexerInfo.m_cpEndOfStatement)
            {
                //We are not at the end of the statement.  Increment to the next
                //non-white space and set the error there.
                while(isspace(*cpSourceLineCurrentPosition) != 0)
                    cpSourceLineCurrentPosition++;

                print_error(__func__, EndOfStatementExpectedError);
                show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                return -EndOfStatementExpectedError;
            }

            //There is nothing after the instruction.  If this is pass two we
            //need to write the opcode to program memory before incrementing the
            //location counter.
            if(s_lexerInfo.m_iPass == PassTwo)
                s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[0].m_iOpCode);

            s_lexerInfo.m_iLocationCounter++;
        }
        else
        {
            //There are operands.  Initialize variables in preparation for
            //getting the operands.
            iProvidedOperands = 0;
            iState = CheckForExpression;

            //Loop until the last character of the statement is used.
            while(cpSourceLineCurrentPosition <= s_lexerInfo.m_cpEndOfStatement)
            {
                //Do work based on state.
                switch(iState)
                {
                    case CheckForExpression :
                        //Check if we already have our maximum operands.
                        if(iProvidedOperands == MAX_OPERANDS)
                        {
                            print_error(__func__, TooManyOperandsSyntaxError);
                            show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                            return -TooManyOperandsSyntaxError;
                        }

                        //Skip white spaces.
                        if(isspace(*cpSourceLineCurrentPosition) == 0)
                        {
                            //We are pointing at the first character of the
                            //operand.
                            aOperands[iProvidedOperands].m_cpStart = cpSourceLineCurrentPosition;

                            //If we haven't yet marked the start of the
                            //expression section then do it now.
                            if(s_lexerInfo.m_cpStatementExpresionStart == NULL)
                                s_lexerInfo.m_cpStatementExpresionStart = cpSourceLineCurrentPosition;

                            //Get address mode type.
                            if(*cpSourceLineCurrentPosition == '#')
                            {
                                //This is an immediate type.
                                iType = Immediate;

                                //Skip past the '#' character and any white
                                //spaces after it.
                                cpSourceLineCurrentPosition++;
                                while(isspace(*cpSourceLineCurrentPosition) != 0)
                                    cpSourceLineCurrentPosition++;
                            }
                            else if(*cpSourceLineCurrentPosition == '[')
                            {
                                //This is a direct page type.
                                iType = DirectPage;

                                //Skip past the '[' character and any white
                                //spaces after it.
                                cpSourceLineCurrentPosition++;
                                while(isspace(*cpSourceLineCurrentPosition) != 0)
                                    cpSourceLineCurrentPosition++;
                            }
                            else if(*cpSourceLineCurrentPosition == '(')
                            {
                                //This is possibly a direct page indirect type.
                                //However, parenthetical grouping is also allowed
                                //for expressions to override standard order of
                                //precedence.  To check go to the end of the
                                //statement section and check for a right
                                //parenthesis.  If a right parenthesis ends the
                                //statement and a left parenthesis starts it then
                                //we can assume it is a direct page indirect.
                                cpTemp = s_lexerInfo.m_cpEndOfStatement;
                                while(isspace(*cpTemp) != 0)
                                    cpTemp--;
                                if(*cpTemp == ')')
                                {
                                    //Confirmed it is a direct page indirect
                                    //type.
                                    iType = DirectPageIndirect;

                                    //Skip past the '(' character and any white
                                    //spaces after it.
                                    cpSourceLineCurrentPosition++;
                                    while(isspace(*cpSourceLineCurrentPosition) != 0)
                                        cpSourceLineCurrentPosition++;
                                }
                                else
                                {
                                    //It is not a direct page indirect type.  The
                                    //only type left is absolute so set that
                                    //type.
                                    iType = Absolute;
                                }
                            }
                            else
                            {
                                //It is not immediate, direct page, or direct
                                //page indirect so it must be absolute.
                                iType = Absolute;
                            }

                            //If this is pass one we don't need to parse the expression.
                            if(s_lexerInfo.m_iPass == PassOne)
                            {
                                //This is pass one so we can skip the expression.
                                //Set the value as zero.
                                aOperands[iProvidedOperands].m_iValue = 0;

                                //Check if there is comma, which is not
                                //immediately preceded by a single quote, which
                                //indicates another operand.
                                while(TRUE)
                                {
                                    //Check for comma and if none found break out
                                    //of loop.
                                    cpTemp = strchr(cpSourceLineCurrentPosition, ',');
                                    if(cpTemp == NULL)
                                        break;

                                    //Comma found.  If it is preceded by a single
                                    //quote then start looking again at the next
                                    //character following the comma.  Otherwise
                                    //break out of the loop.
                                    if(*(cpTemp - 1) == '\'')
                                        cpSourceLineCurrentPosition = cpTemp + 1;
                                    else
                                        break;
                                }

                                if(cpTemp != NULL)
                                {
                                    //Comma found so move the pointer to that
                                    //position.
                                    cpSourceLineCurrentPosition = cpTemp;
                                }
                                else
                                {
                                    //No comma found so move to the NULL
                                    //terminating byte at the end of the line.
                                    cpSourceLineCurrentPosition = s_lexerInfo.m_cpEndOfStatement + 1;
                                }
                            }
                            else
                            {
                                //This is pass two so do a full evaluation of the
                                //expression.
                                iFunctionReturnValue = do_expression(&cpSourceLineCurrentPosition, &(aOperands[iProvidedOperands].m_iValue));
                                if(iFunctionReturnValue != EXIT_SUCCESS)
                                {
                                    //There was an error parsing the expression.
                                    //The statement parser might have returned
                                    //the pointer looking at the NULL terminating
                                    //byte which will make the printed error look
                                    //funny.  If it is pointing at the NULL
                                    //terminating byte then move it back to just
                                    //after the last character.
                                    if(*cpSourceLineCurrentPosition == '\0')
                                    {
                                        cpSourceLineCurrentPosition--;
                                        while(isspace(*cpSourceLineCurrentPosition) != 0)
                                            cpSourceLineCurrentPosition--;
                                        cpSourceLineCurrentPosition++;
                                    }

                                    print_error(__func__, (uint8_t)(-iFunctionReturnValue));
                                    show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                                    return iFunctionReturnValue;
                                }

                                //If this is a direct page or direct page
                                //indirect type then check for the proper closing
                                //symbol.
                                if(iType == DirectPage)
                                {
                                    if(*cpSourceLineCurrentPosition == ']')
                                    {
                                        //We got the closing bracket so increment
                                        //past it.
                                        cpSourceLineCurrentPosition++;
                                    }
                                    else
                                    {
                                        //We did not get the closing bracket.
                                        //The statement parser might have
                                        //returned the pointer looking at the
                                        //NULL terminating byte which will make
                                        //the printed error look funny.  If it is
                                        //pointing at the NULL terminating byte
                                        //then move it back to just after the
                                        //last character.
                                        if(*cpSourceLineCurrentPosition == '\0')
                                        {
                                            cpSourceLineCurrentPosition--;
                                            while(isspace(*cpSourceLineCurrentPosition) != 0)
                                                cpSourceLineCurrentPosition--;
                                            cpSourceLineCurrentPosition++;
                                        }

                                        print_error(__func__, RightBracketExpected);
                                        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                                        return -RightBracketExpected;
                                    }
                                }
                                else if(iType == DirectPageIndirect)
                                {
                                    if(*cpSourceLineCurrentPosition == ')')
                                    {
                                        //We got the closing parenthesis so
                                        //increment past it.
                                        cpSourceLineCurrentPosition++;
                                    }
                                    else
                                    {
                                        //We did not get the closing parenthesis.
                                        //The statement parser might have
                                        //returned the pointer looking at the
                                        //NULL terminating byte which will make
                                        //the printed error look funny.  If it is
                                        //pointing at the NULL terminating byte
                                        //then move it back to just after the
                                        //last character.
                                        if(*cpSourceLineCurrentPosition == '\0')
                                        {
                                            cpSourceLineCurrentPosition--;
                                            while(isspace(*cpSourceLineCurrentPosition) != 0)
                                                cpSourceLineCurrentPosition--;
                                            cpSourceLineCurrentPosition++;
                                        }

                                        print_error(__func__, RightParenthesisExpected);
                                        show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                                        return -RightParenthesisExpected;
                                    }
                                }
                            }

                            //Got expression successfully.  We should never have
                            //an expression that equates to a negative number.
                            if(aOperands[iProvidedOperands].m_iValue < 0)
                            {
                                print_error(__func__, InvalidValueError);
                                show_line_error(aOperands[iProvidedOperands].m_cpStart - s_cpSourceLine);
                                return -InvalidValueError;
                            }

                            //The expression equates to zero or a positive
                            //number.  Increment the number of provided operands.
                            iProvidedOperands++;

                            //Change states to checking for a comma.
                            iState = CheckForComma;
                        }
                        else
                        {
                            //We are pointing at a white space so skip the
                            //character.
                            cpSourceLineCurrentPosition++;
                        }
                        break;
                    case CheckForComma :
                        //We are checking a a comma.  If we are at a white space
                        //then skip it.  Otherwise it must be a comma.
                        if(isspace(*cpSourceLineCurrentPosition) != 0)
                        {
                            //Current character is a white space so skip past it.
                            cpSourceLineCurrentPosition++;
                        }
                        else if(*cpSourceLineCurrentPosition == ',')
                        {
                            //We are pointing at a comma.  Change the state and
                            //skip past the comma.
                            iState = CheckForExpression;
                            cpSourceLineCurrentPosition++;
                        }
                        else
                        {
                            //Neither a white space nor a comma.  This is an
                            //error.
                            print_error(__func__, EndOfStatementExpectedError);
                            show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                            return -EndOfStatementExpectedError;
                        }
                        break;
                    default :
                        //Should never get here but put in for good programming practice.
                        break;
                }
            }

            //We are out of the while() loop so we are done with the statement section.
            //We know that the last character of the statement section is not a
            //whitespace because all trailing white spaces were removed by the parser.
            //If the state is looking for a comma this is success.  Anything else is an
            //error.
            if(iState != CheckForComma)
            {
                print_error(__func__, UnexpectedEndOfStatementSyntaxError);
                show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
                return -UnexpectedEndOfStatementSyntaxError;
            }

            //Find the correct opcode for this mnemonic based on the operand type
            //and number of operands.
            bFoundType = FALSE;
            for(iOpcodeInfoIndex = 0; iOpcodeInfoIndex < s_aInstructionTable[iInstructionIndex].m_iNumberOfTypes; iOpcodeInfoIndex++)
            {
                if(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iType == iType)
                {
                    //Found a match for the type.  Set the flag and check if the
                    //number of operands match.
                    bFoundType = TRUE;
                    if(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iNumberOfOperands == iProvidedOperands)
                        break;
                }
            }

            //We are out of the for() loop.  If we made it all the way to the end
            //then a match was not found.
            if(iOpcodeInfoIndex == s_aInstructionTable[iInstructionIndex].m_iNumberOfTypes)
            {
                if(bFoundType == FALSE)
                {
                    //The addressing mode type provided is not supported by this
                    //instruction.
                    print_error(__func__, TypeNotSupported);
                    show_line_error(s_lexerInfo.m_cpStatementMnemonicStart - s_cpSourceLine);
                    return -TypeNotSupported;
                }
                else
                {
                    //Incorrect number of operands.  If no operands were provided
                    //point at the mnemonic.  If at least one operand was provided
                    //point at the start of the operands.
                    print_error(__func__, IncorrectNumberOfOperandsError);
                    show_line_error((iProvidedOperands == 0) ? s_lexerInfo.m_cpStatementMnemonicStart - s_cpSourceLine : s_lexerInfo.m_cpStatementExpresionStart - s_cpSourceLine);
                    return -IncorrectNumberOfOperandsError;
                }
            }

            //We found a match.  If this is pass one we need only increment the
            //location counter by the number of bytes the instruction takes.
            //Otherwise, we need to check the operands for proper values and
            //store the instruction bytes.
            if(s_lexerInfo.m_iPass == PassOne)
            {
                s_lexerInfo.m_iLocationCounter += s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iLength;
            }
            else
            {
                //Do work based on the type.
                switch(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iType)
                {
                    case Immediate :
                    case DirectPage :
                        //For each of these types there is a single operand to
                        //check that must be in the range of 0 to 255 inclusive.
                        //Negative numbers were already checked so we only need
                        //test the high range.
                        if(aOperands[0].m_iValue > 255)
                        {
                            print_error(__func__, InvalidValueError);
                            show_line_error(aOperands[0].m_cpStart - s_cpSourceLine);
                            return -InvalidValueError;
                        }

                        //The operand value is valid.  Write the opcode to
                        //program memory.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iOpCode);
                        s_lexerInfo.m_iLocationCounter++;

                        //Write the operand to program memory.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(aOperands[0].m_iValue);
                        s_lexerInfo.m_iLocationCounter++;
                        break;
                    case DirectPageIndirect :
                    case Absolute :
                        //For each of these types there is a single operand to
                        //check that must be in the range of 0 to 65535
                        //inclusive.  Negative numbers were already checked so we
                        //only need test the high range.
                        if(aOperands[0].m_iValue > 65535)
                        {
                            print_error(__func__, InvalidValueError);
                            show_line_error(aOperands[0].m_cpStart - s_cpSourceLine);
                            return -InvalidValueError;
                        }

                        //The operand value is valid.  Write the opcode to
                        //program memory.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(s_aInstructionTable[iInstructionIndex].m_pOpcodeInfo[iOpcodeInfoIndex].m_iOpCode);
                        s_lexerInfo.m_iLocationCounter++;

                        //Write the least significant byte of the operand to
                        //program memory.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(aOperands[0].m_iValue);
                        s_lexerInfo.m_iLocationCounter++;

                        //Write the most significant byte of the operand to
                        //program memory.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(aOperands[0].m_iValue >> 8);
                        s_lexerInfo.m_iLocationCounter++;
                        break;
                    default :
                        //Should never get here but put in for good programming practice.
                        break;
                }
            }
        }
    }
    else
    {
        //It is neither a directive nor mnemonic so we assume it is a symbol.
        //Mark the start and end of this symbol.
        s_lexerInfo.m_cpStatementSymbolStart = s_lexerInfo.m_cpStartOfStatement;
        s_lexerInfo.m_cpStatementSymbolEnd = cpSourceLineCurrentPosition - 1;

        //We should have an EQU directive name following the symbol.  Skip all
        //white spaces.
        while(isspace(*cpSourceLineCurrentPosition) != 0)
            cpSourceLineCurrentPosition++;

        //Check if we are pointing at "EQU".
        if(strncmp(cpSourceLineCurrentPosition, EQU_DIRECTIVE_TEXT, sizeof(EQU_DIRECTIVE_TEXT) - NULL_TERMINATING_BYTE_LENGTH) != 0)
        {
            print_error(__func__, ExpectedEquAfterSymbolSyntaxError);
            show_line_error(cpSourceLineCurrentPosition - s_cpSourceLine);
            return -ExpectedEquAfterSymbolSyntaxError;
        }

        //Found EQU after the symbol.  Mark the start and end of this directive
        //mnemonic and then increment past EQU.
        s_lexerInfo.m_cpStatementMnemonicStart = cpSourceLineCurrentPosition;
        s_lexerInfo.m_cpStatementMnemonicEnd = cpSourceLineCurrentPosition + (sizeof(EQU_DIRECTIVE_TEXT) - NULL_TERMINATING_BYTE_LENGTH) - 1;
        cpSourceLineCurrentPosition += (sizeof(EQU_DIRECTIVE_TEXT) - NULL_TERMINATING_BYTE_LENGTH);

        //Execute the EQU directive.
        iFunctionReturnValue = do_equ_directive(cpSourceLineCurrentPosition, caText);
        return (iFunctionReturnValue == EXIT_SUCCESS) ? EQU_DIRECTIVE_SUCCESS : iFunctionReturnValue;
    }

    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  search_directive_table
 * Function Description:  Searches the directive table for a specific directive.
 * Parameters:
 * cpDirective - Character pointer to the directive for which to search.
 * Returns:  If found the index into the directive table is returned.  Otherwise,
 *           -1 is returned if not found.
------------------------------------------------------------------------------*/
static int search_directive_table(const char* cpDirective)
{
    uint32_t iIndex;

    for(iIndex = 0; iIndex < (sizeof(s_aDirectiveTable) / sizeof(DirectiveInfo)); iIndex++)
    {
        if(strcmp(cpDirective, s_aDirectiveTable[iIndex].m_cpDirective) == 0)
            return (int)iIndex;
    }

    return -1;
}

/*------------------------------------------------------------------------------
 * Function name:  search_instruction_table
 * Function Description:  Searches the instruction table for a specific mnemonic.
 * Parameters:
 * cpMnemonic - Character pointer to the mnemonic for which to search.
 * Returns:  If found the index into the instruction table is returned.
 *           Otherwise, -1 is returned if not found.
------------------------------------------------------------------------------*/
static int search_instruction_table(const char* cpMnemonic)
{
    uint32_t iIndex;

    for(iIndex = 0; iIndex < (sizeof(s_aInstructionTable) / sizeof(InstructionInfo)); iIndex++)
    {
        if(strcmp(cpMnemonic, s_aInstructionTable[iIndex].m_cpMnemonic) == 0)
            return (int)iIndex;
    }

    return -1;
}

/*------------------------------------------------------------------------------
 * Function name:  do_byte_directive
 * Function Description:  Attempts to parse out the number(s) following the BYTE
 *                        directive.
 * Parameters:
 * cpCurrentStatementPosition - Character pointer to the current position within
 *                              the statement section.
 * Returns:  Zero for success and negative number for failure.
------------------------------------------------------------------------------*/
static int do_byte_directive(char* cpCurrentStatementPosition)
{
    uint8_t iState;

    int iFunctionReturnValue;

    uint32_t iCounter;

    ExpressionInfo expressionInfo;

    //Initialize state.
    iState = CheckForExpression;

    //Loop until the last character of the statement is used.
    while(cpCurrentStatementPosition <= s_lexerInfo.m_cpEndOfStatement)
    {
        //Do work based on state.
        switch(iState)
        {
            case CheckForExpression :
                //Get the expression.  First eliminate leading white spaces.
                while(isspace(*cpCurrentStatementPosition) != 0)
                    cpCurrentStatementPosition++;

                //We are pointing at the first character of the expression.
                expressionInfo.m_cpStart = cpCurrentStatementPosition;

                //If we haven't yet marked the start of the expression section
                //then do it now.
                if(s_lexerInfo.m_cpStatementExpresionStart == NULL)
                    s_lexerInfo.m_cpStatementExpresionStart = cpCurrentStatementPosition;

                //If this is pass one we don't need to parse the expression.
                if(s_lexerInfo.m_iPass == PassOne)
                {
                    //This is pass one so we can skip the expression.  Set the
                    //value as zero.
                    expressionInfo.m_iValue = 0;

                    //Assume there is only one byte.  If a literal string is
                    //detected we will get a new byte count.
                    iCounter = 1;

                    //Check if there is comma which indicates another expression.
                    //Care must be taken as a comma may be part of a literal
                    //character or literal string.
                    while(*cpCurrentStatementPosition != ',' && cpCurrentStatementPosition <= s_lexerInfo.m_cpEndOfStatement)
                    {
                        if(*cpCurrentStatementPosition == '\'')
                        {
                            //We reached a single quote.  Skip it and the
                            //character immediately following.
                            cpCurrentStatementPosition += 2;
                        }
                        else if(*cpCurrentStatementPosition == '"')
                        {
                            //We reached a double quote, skip over it.
                            cpCurrentStatementPosition++;

                            //Loop until the next double quote, or end of
                            //statement, is reached.  Increment the counter for
                            //each character as each character takes up a byte in
                            //memory.
                            iCounter = 0;
                            while(*cpCurrentStatementPosition != '"' && cpCurrentStatementPosition <= s_lexerInfo.m_cpEndOfStatement)
                            {
                                //There can be escaped characters in a string and those need to
                                //be treated as a single byte.  These characters are preceded by
                                //a backslash character.
                                if(*cpCurrentStatementPosition == '\\')
                                {
                                    //Backslash character found.  Skip past it
                                    //and the character immediately following.
                                    cpCurrentStatementPosition += 2;

                                    //Count as a single byte.
                                    iCounter++;
                                }
                                else
                                {
                                    //Not a backslash so skip past the character
                                    //and count it as one byte.
                                    cpCurrentStatementPosition++;
                                    iCounter++;
                                }
                            }

                            //If we are pointing at a double quote then skip over it.
                            if(*cpCurrentStatementPosition == '"')
                                cpCurrentStatementPosition++;
                        }
                        else
                        {
                            //Neither a single quote or double quote so check for
                            //a comma and if found break out.
                            if(*cpCurrentStatementPosition == ',')
                                break;
                            else
                                cpCurrentStatementPosition++;
                        }
                    }

                    //Increment the location counter to the next byte.
                    s_lexerInfo.m_iLocationCounter += iCounter;

                    //Switch state to checking for a comma.
                    iState = CheckForComma;
                }
                else
                {
                    //This is pass two so do a full evaluation.  It can be an
                    //expression or a literal string.
                    if(*cpCurrentStatementPosition != '"')
                    {
                        //It is an expression.
                        iFunctionReturnValue = do_expression(&cpCurrentStatementPosition, &(expressionInfo.m_iValue));
                        if(iFunctionReturnValue != EXIT_SUCCESS)
                        {
                            print_error(__func__, (uint8_t)(-iFunctionReturnValue));
                            show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
                            return iFunctionReturnValue;
                        }

                        //Number retrieved, check that it is a byte.
                        if(expressionInfo.m_iValue < 0 || expressionInfo.m_iValue > 255)
                        {
                            print_error(__func__, InvalidValueError);
                            show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
                            return -InvalidValueError;
                        }

                        //Number value is valid, store it at the current location.
                        s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(expressionInfo.m_iValue);

                        //Increment the location counter to the next byte.
                        s_lexerInfo.m_iLocationCounter++;
                    }
                    else
                    {
                        //It is a literal string.  Skip over the double quote.
                        cpCurrentStatementPosition++;

                        //Loop until the next double quote, or end of statement,
                        //is reached.  Store the value of each character.
                        while(*cpCurrentStatementPosition != '"' && cpCurrentStatementPosition <= s_lexerInfo.m_cpEndOfStatement)
                        {
                            //If we are pointing a a backslash then we have to
                            //process a special character.
                            if(*cpCurrentStatementPosition == '\\')
                            {
                                //It is a special character.  Skip past the
                                //backslash character.
                                cpCurrentStatementPosition++;

                                //Get the value for the escaped character.
                                s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)get_esc_character(*cpCurrentStatementPosition);
                                if(s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] == 0)
                                {
                                    print_error(__func__, InvalidCharacterSyntaxError);
                                    show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
                                    return -InvalidCharacterSyntaxError;
                                }

                                //Increment the location counter to the next byte.
                                s_lexerInfo.m_iLocationCounter++;

                                //Increment past the escaped character.
                                cpCurrentStatementPosition++;
                            }
                            else
                            {
                                //Not a special character so just process the
                                //character as is.
                                s_lexerInfo.m_iaProgramMemory[s_lexerInfo.m_iLocationCounter] = (int16_t)(*cpCurrentStatementPosition);

                                //Increment the location counter to the next byte.
                                s_lexerInfo.m_iLocationCounter++;

                                //Increment past the character.
                                cpCurrentStatementPosition++;
                            }
                        }

                        //If we are pointing at a double quote then skip over it.
                        if(*cpCurrentStatementPosition == '"')
                            cpCurrentStatementPosition++;
                    }

                    //Switch state to checking for a comma.
                    iState = CheckForComma;
                }
                break;
            case CheckForComma :
                //We are checking a a comma.  If we are at a white space
                //then skip it.  Otherwise it must be a comma.
                if(isspace(*cpCurrentStatementPosition) != 0)
                {
                    cpCurrentStatementPosition++;
                }
                else if(*cpCurrentStatementPosition == ',')
                {
                    iState = CheckForExpression;
                    cpCurrentStatementPosition++;
                }
                else
                {
                    print_error(__func__, EndOfStatementExpectedError);
                    show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
                    return -EndOfStatementExpectedError;
                }
                break;
            default :
                //Should never get here but put in for good programming practice.
                break;
        }
    }

    //We are out of the while() loop so we are done with the statement section.
    //We know that the last character of the statement section is not a
    //whitespace because all trailing white spaces were removed by the parser.
    //If the state is looking for a comma this is success.  Anything else is an
    //error.
    if(iState == CheckForComma)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        print_error(__func__, UnexpectedEndOfStatementSyntaxError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -UnexpectedEndOfStatementSyntaxError;
    }
}

/*------------------------------------------------------------------------------
 * Function name:  do_end_directive
 * Function Description:  Executes the END directive.
 * Parameters:
 * cpCurrentStatementPosition - Character pointer to the current position within
 *                              the statement section.
 * Returns:  Zero for success and negative number for failure.
------------------------------------------------------------------------------*/
static int do_end_directive(char* cpCurrentStatementPosition)
{
    //The END directive must be alone on the line, no label, but a comment is
    //allowed.
    if(s_lexerInfo.m_cpStartOfLabel != NULL)
    {
        //There is a label on the line.
        print_error(__func__, EndDirectiveNotAloneError);
        show_line_error(s_lexerInfo.m_cpStartOfLabel - s_cpSourceLine);
        return -EndDirectiveNotAloneError;
    }
    else if((s_lexerInfo.m_cpEndOfStatement + 1) != cpCurrentStatementPosition)
    {
        //There is more after the END directive.  We would be pointing at a white
        //space at the moment.  Increment to the next non-white space.
        while(isspace(*cpCurrentStatementPosition) != 0)
            cpCurrentStatementPosition++;

        print_error(__func__, EndOfStatementExpectedError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -EndOfStatementExpectedError;
    }
    else
    {
        //We can accept the END directive.
        return EXIT_SUCCESS;
    }
}

/*------------------------------------------------------------------------------
 * Function name:  do_equ_directive
 * Function Description:  Attempts to parse out the number following the EQU
 *                        directive.
 * Parameters:
 * cpCurrentStatementPosition - Character pointer to the current position within
 *                              the statement section.
 * cpSymbol - Character pointer to the symbol that precedes the EQU directive.
 * Returns:  Zero for success and negative number for failure.
------------------------------------------------------------------------------*/
static int do_equ_directive(char* cpCurrentStatementPosition, char* cpSymbol)
{
    int iFunctionReturnValue;

    ExpressionInfo expressionInfo;

    BSTreeNode* pNode;

    //We need an expression after EQU.  If we are beyond the end of statement
    //then this is an error.
    if(cpCurrentStatementPosition > s_lexerInfo.m_cpEndOfStatement)
    {
        print_error(__func__, UnexpectedEndOfStatementSyntaxError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -UnexpectedEndOfStatementSyntaxError;
    }

    //There is something beyond EQU.  Eliminate leading white spaces.
    while(isspace(*cpCurrentStatementPosition) != 0)
        cpCurrentStatementPosition++;

    //This should be the start of an expression.
    expressionInfo.m_cpStart = cpCurrentStatementPosition;

    //If we haven't yet marked the start of the expression then do it now.
    if(s_lexerInfo.m_cpStatementExpresionStart == NULL)
        s_lexerInfo.m_cpStatementExpresionStart = cpCurrentStatementPosition;

    iFunctionReturnValue = do_expression(&cpCurrentStatementPosition, &(expressionInfo.m_iValue));
    if(iFunctionReturnValue != EXIT_SUCCESS)
    {
        print_error(__func__, (uint8_t)(-iFunctionReturnValue));
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return iFunctionReturnValue;
    }

    //We successfully parsed the expression.  When the expression parser returns
    //it should be pointing at a NULL pointer or the start of the comment
    //section.  Anything else is an error.
    if(*cpCurrentStatementPosition == '\0' || *cpCurrentStatementPosition == ';')
    {
        //The number must be positive.
        if(expressionInfo.m_iValue < 0)
        {
            print_error(__func__, InvalidValueError);
            show_line_error(expressionInfo.m_cpStart - s_cpSourceLine);
            return -InvalidValueError;
        }

        //We have the number.  We only care about storing it in the symbol table
        //during pass one.
        if(s_lexerInfo.m_iPass == PassOne)
        {
            //First check for duplicate symbol.
            pNode = bstree_search(s_pSymbolTableRoot, cpSymbol, bstree_key_compare);
            if(pNode != NULL)
            {
                //The symbol is a duplicate.  Print the error and return the
                //proper error value.
                print_error(__func__, DuplicateSymbolError);
                show_line_error(s_lexerInfo.m_cpStartOfStatement - s_cpSourceLine);
                return -DuplicateSymbolError;
            }

            //The symbol is unique so it can be inserted into the symbol tree.
            pNode = bstree_insert(&s_pSymbolTableRoot, cpSymbol, strlen(cpSymbol) + NULL_TERMINATING_BYTE_LENGTH, bstree_key_compare);
            if(pNode == NULL)
            {
                print_error(__func__, MallocReturnedNull);
                return -MallocReturnedNull;
            }

            //Insert was successful.  We need to add the value of this symbol.
            //The node includes a void pointer to a data element which can be
            //anything.  We will use it an unsigned int pointer.
            pNode->m_vpDataElement = malloc(sizeof(uint32_t));
            if(pNode->m_vpDataElement == NULL)
            {
                print_error(__func__, MallocReturnedNull);
                return -MallocReturnedNull;
            }

            //Memory allocated successfully.  Store the number as the value.
            *(uint32_t*)(pNode->m_vpDataElement) = (uint32_t)(expressionInfo.m_iValue);

            //Check if the length of this symbol is larger then the current
            //longest symbol.
            s_lexerInfo.m_iLargestSymbolLength = bmc_max(strlen(cpSymbol), s_lexerInfo.m_iLargestSymbolLength);
        }
    }
    else
    {
        //There is more.
        print_error(__func__, EndOfStatementExpectedError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -EndOfStatementExpectedError;
    }

    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  do_org_directive
 * Function Description:  Attempts to parse out the number following the ORG
 *                        directive.
 * Parameters:
 * cpCurrentStatementPosition - Character pointer to the current position within
 *                              the statement section.
 * Returns:  Zero for success and negative number for failure.
------------------------------------------------------------------------------*/
static int do_org_directive(char* cpCurrentStatementPosition)
{
    int iFunctionReturnValue;

    ExpressionInfo expressionInfo;

    //We need an expression after ORG.  If we are beyond the end of statement
    //then this is an error.
    if(cpCurrentStatementPosition > s_lexerInfo.m_cpEndOfStatement)
    {
        print_error(__func__, UnexpectedEndOfStatementSyntaxError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -UnexpectedEndOfStatementSyntaxError;
    }

    //There is something beyond ORG, get the expression.  Eliminate leading white
    //spaces.
    while(isspace(*cpCurrentStatementPosition) != 0)
        cpCurrentStatementPosition++;

    //This should be the start of an expression.
    expressionInfo.m_cpStart = cpCurrentStatementPosition;

    //If we haven't yet marked the start of the expression then do it now.
    if(s_lexerInfo.m_cpStatementExpresionStart == NULL)
        s_lexerInfo.m_cpStatementExpresionStart = cpCurrentStatementPosition;

    iFunctionReturnValue = do_expression(&cpCurrentStatementPosition, &(expressionInfo.m_iValue));
    if(iFunctionReturnValue != EXIT_SUCCESS)
    {
        print_error(__func__, (uint8_t)(-iFunctionReturnValue));
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return iFunctionReturnValue;
    }

    //We successfully parsed the expression.  When do_expression returns we
    //should be pointing at either a NULL terminating byte or the start of the
    //comment section.
    if(*cpCurrentStatementPosition == '\0' || *cpCurrentStatementPosition == ';')
    {
        //We have the number, check that it is within the program memory.
        if(expressionInfo.m_iValue < 0 || expressionInfo.m_iValue >= MAX_PROGRAM_MEMORY)
        {
            print_error(__func__, InvalidValueError);
            show_line_error(expressionInfo.m_cpStart - s_cpSourceLine);
            return -InvalidValueError;
        }

        //The value is within program memory.  Check if the location counter will
        //be moved backwards.
        if((uint32_t)(expressionInfo.m_iValue) < s_lexerInfo.m_iLocationCounter)
        {
            print_error(__func__, LocationCounterBackwards);
            show_line_error(expressionInfo.m_cpStart - s_cpSourceLine);
            return -LocationCounterBackwards;
        }

        //Value is valid, update the location counter.
        s_lexerInfo.m_iLocationCounter = (uint32_t)(expressionInfo.m_iValue);
    }
    else
    {
        //There is more.
        print_error(__func__, EndOfStatementExpectedError);
        show_line_error(cpCurrentStatementPosition - s_cpSourceLine);
        return -EndOfStatementExpectedError;
    }

    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------------------
 * Function name:  show_line_error
 * Function Description:  Prints information about where an error occurred in a
 *                        source file line.
 * Parameters:
 * iSourceLineErrorIndex - The index into the source line where the error
 *                         occurred.
 * Returns:  None.
------------------------------------------------------------------------------*/
static void show_line_error(uint32_t iSourceLineErrorIndex)
{
    printf("Line %u:\n", s_lexerInfo.m_iSourceLineNumber);
    printf("%s", s_cpSourceLine);
    printf( "%*s\n", iSourceLineErrorIndex + 1, "^" );
}

/*------------------------------------------------------------------------------
 * Function name:  print_symbol_to_table
 * Function Description:  Prints symbol information to the symbol table in the
 *                        listing file.
 * Parameters:
 * pNode - Pointer to the node from which to print the symbol name and value.
 * Returns:  None.
------------------------------------------------------------------------------*/
static void print_symbol_to_table(BSTreeNode* pNode)
{
    char caText[LISTING_FILE_MAX_COLUMNS + NULL_TERMINATING_BYTE_LENGTH];

    //Print the symbol information to the listing file if the symbol table and
    //listing file are enabled.
    if(is_symbol_table_enabled() == TRUE && is_listing_file_enabled() == TRUE)
    {
        snprintf(caText, sizeof(caText), "%-*s%08X", s_iStatementSymbolFieldLength, (char*)(pNode->m_vpKey), *((uint32_t*)(pNode->m_vpDataElement)));
        write_line_to_listing_file(caText);
    }
}
