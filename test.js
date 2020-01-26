let code = `
                    case Lexer::ADD_TOKEN:
                        bytecode += (char)OPID::ADD;
                        break;
                    case Lexer::SUB_TOKEN:
                        bytecode += (char)OPID::SUB;
                        break;
                    case Lexer::MUL_TOKEN:
                        bytecode += (char) OPID::MUL;
                        break;
                    case Lexer::DIV_TOKEN:
                        bytecode += (char) OPID::DIV;
                        break;
                    case Lexer::MOD_TOKEN:
                        bytecode += (char) OPID::MOD;
                        break;
                    case Lexer::EQUAL_TOKEN:
                        bytecode += (char)OPID::EQ;
                        break;
                    case Lexer::NOT_EQUAL_TOKEN:
                        bytecode += (char)OPID::NEQ;
                        break;
                    case Lexer::LEQUAL_TOKEN:
                        bytecode += (char)OPID::LEQ;
                        break;
                    case Lexer::MEQUAL_TOKEN:
                        bytecode += (char)OPID::MEQ;
                        break;
                    case Lexer::LESS_TOKEN:
                        bytecode += (char)OPID::LESS;
                        break;
                    case Lexer::MORE_TOKEN:
                        bytecode += (char)OPID::MORE;
                        break;
                    case Lexer::MAND_TOKEN:
                        bytecode += (char)OPID::MAND;
                        break;
                    case Lexer::MOR_TOKEN:
                        bytecode += (char)OPID::MOR;
                        break;
                    case Lexer::MNOT_TOKEN:
                        bytecode += (char)OPID::MNOT;
                        break;
                    case Lexer::MXOR_TOKEN:
                        bytecode += (char)OPID::MXOR;
                        break;
                    case Lexer::LAND_TOKEN:
                        bytecode += (char)OPID::LAND;
                        break;
                    case Lexer::LOR_TOKEN:
                        bytecode += (char)OPID::LOR;
                        break;
                    case Lexer::LNOT_TOKEN:
                        bytecode += (char)OPID::LNOT;
                        break;
                    case Lexer::POWER_TOKEN:
                        bytecode += (char)OPID::POW;
                        break;`;
code = code.replace(/Lexer::([a-zA-Z0-9]+)_TOKEN:/g, "Lexer::$1_TO_TOKEN:");
console.log(code);