//===--- UnwrappedLineParser.h - Format C++ code ----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the UnwrappedLineParser,
/// which turns a stream of tokens into UnwrappedLines.
///
/// This is EXPERIMENTAL code under heavy development. It is not in a state yet,
/// where it can be used to format real code.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_FORMAT_UNWRAPPED_LINE_PARSER_H
#define LLVM_CLANG_FORMAT_UNWRAPPED_LINE_PARSER_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Format/Format.h"
#include "clang/Lex/Lexer.h"

namespace clang {
namespace format {

/// \brief A wrapper around a \c Token storing information about the
/// whitespace characters preceeding it.
struct FormatToken {
  FormatToken()
      : NewlinesBefore(0), HasUnescapedNewline(false), WhiteSpaceLength(0) {
  }

  /// \brief The \c Token.
  Token Tok;

  /// \brief The number of newlines immediately before the \c Token.
  ///
  /// This can be used to determine what the user wrote in the original code
  /// and thereby e.g. leave an empty line between two function definitions.
  unsigned NewlinesBefore;

  /// \brief Whether there is at least one unescaped newline before the \c
  /// Token.
  bool HasUnescapedNewline;

  /// \brief The location of the start of the whitespace immediately preceeding
  /// the \c Token.
  ///
  /// Used together with \c WhiteSpaceLength to create a \c Replacement.
  SourceLocation WhiteSpaceStart;

  /// \brief The length in characters of the whitespace immediately preceeding
  /// the \c Token.
  unsigned WhiteSpaceLength;
};

/// \brief An unwrapped line is a sequence of \c Token, that we would like to
/// put on a single line if there was no column limit.
///
/// This is used as a main interface between the \c UnwrappedLineParser and the
/// \c UnwrappedLineFormatter. The key property is that changing the formatting
/// within an unwrapped line does not affect any other unwrapped lines.
struct UnwrappedLine {
  UnwrappedLine() : Level(0), InPPDirective(false) {
  }

  /// \brief The \c Token comprising this \c UnwrappedLine.
  SmallVector<FormatToken, 16> Tokens;

  /// \brief The indent level of the \c UnwrappedLine.
  unsigned Level;

  /// \brief Whether this \c UnwrappedLine is part of a preprocessor directive.
  bool InPPDirective;
};

class UnwrappedLineConsumer {
public:
  virtual ~UnwrappedLineConsumer() {
  }
  virtual void consumeUnwrappedLine(const UnwrappedLine &Line) = 0;
};

class FormatTokenSource {
public:
  virtual ~FormatTokenSource() {
  }
  virtual FormatToken getNextToken() = 0;
};

class UnwrappedLineParser {
public:
  UnwrappedLineParser(const FormatStyle &Style, FormatTokenSource &Tokens,
                      UnwrappedLineConsumer &Callback);

  /// Returns true in case of a structural error.
  bool parse();

private:
  bool parseLevel();
  bool parseBlock(unsigned AddLevels = 1);
  void parsePPDirective();
  void parseComments();
  void parseStatement();
  void parseParens();
  void parseIfThenElse();
  void parseForOrWhileLoop();
  void parseDoWhile();
  void parseLabel();
  void parseCaseLabel();
  void parseSwitch();
  void parseNamespace();
  void parseAccessSpecifier();
  void parseEnum();
  void addUnwrappedLine();
  bool eof() const;
  void nextToken();

  UnwrappedLine Line;
  FormatToken FormatTok;

  const FormatStyle &Style;
  FormatTokenSource &Tokens;
  UnwrappedLineConsumer &Callback;
};

}  // end namespace format
}  // end namespace clang

#endif  // LLVM_CLANG_FORMAT_UNWRAPPED_LINE_PARSER_H
