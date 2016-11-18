//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Aug  8 12:24:49 PDT 2015
// Last Modified: Wed Aug 12 10:42:07 PDT 2015
// Filename:      HumAddress.h
// URL:           https://github.com/craigsapp/humlib/blob/master/include/HumAddress.h
// Syntax:        C++11
// vim:           ts=3 noexpandtab
//
// Description:   Used to store Humdrum text lines and analytic markup
//                of the line.
//

#ifndef _HUMDRUMLINE_H
#define _HUMDRUMLINE_H

#include <iostream>

#include "HumdrumToken.h"
#include "HumHash.h"

using namespace std;

namespace hum {

class HumdrumFile;

// START_MERGE

class HumdrumLine : public string, public HumHash {
	public:
		         HumdrumLine          (void);
		         HumdrumLine          (const string& aString);
		         HumdrumLine          (const char* aString);
		        ~HumdrumLine          ();

		bool     isComment              (void) const;
		bool     isCommentLocal         (void) const;
		bool     isLocalComment (void) const { return isCommentLocal(); }
		bool     isCommentGlobal        (void) const;
		bool     isReference            (void) const;
		string   getReferenceKey        (void) const;
		string   getReferenceValue      (void) const;
		bool     isGlobalComment (void) const { return isCommentGlobal(); }
		bool     isExclusive            (void) const;
		bool     isExclusiveInterpretation (void) const { return isExclusive(); }
		bool     isTerminator           (void) const;
		bool     isInterp               (void) const;
		bool     isInterpretation       (void) const { return isInterp(); }
		bool     isBarline              (void) const;
		bool     isData                 (void) const;
		bool     isAllNull              (void) const;
		bool     isAllRhythmicNull      (void) const;
		bool     isEmpty                (void) const;
		bool     isBlank                (void) const { return isEmpty(); }
		bool     isManipulator          (void) const;
		bool     hasSpines              (void) const;
		bool     isGlobal               (void) const;
		HTp      token                  (int index) const;
		void     getTokens              (vector<HTp>& list);
		int      getTokenCount          (void) const;
		int      getFieldCount          (void) const { return getTokenCount(); }
		string   getTokenString         (int index) const;
		bool     equalChar              (int index, char ch) const;
		char     getChar                (int index) const;
		bool     isKernBoundaryStart    (void) const;
		bool     isKernBoundaryEnd      (void) const;
		ostream& printSpineInfo         (ostream& out = cout);
		ostream& printTrackInfo         (ostream& out = cout);
		ostream& printDataTypeInfo      (ostream& out = cout);
		ostream& printDurationInfo      (ostream& out = cout);
		ostream& printCsv               (ostream& out = cout,
		                                 const string& separator = ",");
		ostream& printXml               (ostream& out = cout, int level = 0,
		                                 const string& indent = "\t");
		string   getXmlId               (const string& prefix = "") const;
		string   getXmlIdPrefix         (void) const;
		void     createLineFromTokens   (void);
		int      getLineIndex           (void) const;
		int      getLineNumber          (void) const;
		HumdrumFile* getOwner           (void);

		HumNum   getDuration            (void) const;
		HumNum   getDurationFromStart   (void) const;
		HumNum   getDurationToEnd       (void) const;
		HumNum   getDurationFromBarline (void) const;
		HumNum   getDurationToBarline   (void) const;
		HumNum   getBarlineDuration     (void) const;

		HumNum   getDuration            (HumNum scale) const;
		HumNum   getDurationFromStart   (HumNum scale) const;
		HumNum   getDurationToEnd       (HumNum scale) const;
		HumNum   getDurationFromBarline (HumNum scale) const;
		HumNum   getDurationToBarline   (HumNum scale) const;
		HumNum   getBarlineDuration     (HumNum scale) const;

		HumNum   getBeat                (HumNum beatdur = "1") const;
		HumNum   getBeat                (string beatrecip = "4") const;
		HTp      getTrackStart     (int track) const;
		void     setLineFromCsv         (const char* csv,
		                                 const string& separator = ",");
		void     setLineFromCsv         (const string& csv,
		                                 const string& separator = ",");

		// low-level editing functions (need to re-analyze structure after using)
		void     appendToken            (HTp token);
		void     appendToken            (const HumdrumToken& token);
		void     appendToken            (const string& token);
		void     appendToken            (const char* token);

		void     insertToken            (int index, HTp token);
		void     insertToken            (int index, const HumdrumToken& token);
		void     insertToken            (int index, const string& token);
		void     insertToken            (int index, const char* token);

		void     setDuration            (HumNum aDur);
		void     setDurationFromStart   (HumNum dur);
		void     setDurationFromBarline (HumNum dur);
		void     setDurationToBarline   (HumNum dur);

	protected:
		bool     analyzeTracks          (string& err);
		bool     analyzeTokenDurations  (string& err);
		void     setLineIndex           (int index);
		void     clear                  (void);
		void     setOwner               (void* hfile);
		int      createTokensFromLine   (void);
		void     setParameters          (HumdrumLine* pLine);
		void     setParameters          (const string& pdata);

	private:

		//
		// State variables managed by the HumdrumLine class:
		//

		// lineindex: Used to store the index number of the HumdrumLine in
		// the owning HumdrumFile object.
		// This variable is filled by HumdrumFileStructure::analyzeLines().
		int lineindex;

		// tokens: Used to store the individual tab-separated token fields
		// on a line.  These are prepared automatically after reading in
		// a full line of text (which is accessed throught the string parent
		// class).  If the full line is changed, the tokens are not updated
		// automatically -- use createTokensFromLine().  Likewise the full
		// text line is not updated if any tokens are changed -- use
		// createLineFromTokens() in that case.  The second case is more
		// useful: you can read in a HumdrumFile, tweak the tokens, then
		// reconstruct the full line and print out again.
		// This variable is filled by HumdrumFile::read().
		// The contents of this vector should be deleted when deconstructing
		// a HumdrumLine object.
		vector<HumdrumToken*> tokens;

		// duration: This is the "duration" of a line.  The duration is
		// equal to the minimum time unit of all durational tokens on the
		// line.  This also includes null tokens when the duration of a
		// previous note in a previous spine is ending on the line, so it is
		// not just the minimum duration on the line.
		// This variable is filled by HumdrumFileStructure::analyzeRhythm().
		HumNum duration;

		// durationFromStart: This is the cumulative duration of all lines
		// prior to this one in the owning HumdrumFile object.  For example,
		// the first notes in a score start at time 0, If the duration of the
		// first data line is 1 quarter note, then the durationFromStart for
		// the second line will be 1 quarter note.
		// This variable is filled by HumdrumFileStructure::analyzeRhythm().
		HumNum durationFromStart;

		// durationFromBarline: This is the cumulative duration from the
		// last barline to the current data line.
		// This variable is filled by HumdrumFileStructure::analyzeMeter().
		HumNum durationFromBarline;

		// durationToBarline: This is the duration from the start of the
		// current line to the next barline in the owning HumdrumFile object.
		// This variable is filled by HumdrumFileStructure::analyzeMeter().
		HumNum durationToBarline;

		// owner: This is the HumdrumFile which manages the given line.
		void* owner;

	friend class HumdrumFileBase;
	friend class HumdrumFileStructure;
	friend class HumdrumFileContent;
	friend class HumdrumFile;
};

ostream& operator<< (ostream& out, HumdrumLine& line);


// END_MERGE

} // end namespace hum

#endif /* _HUMDRUMLINE */



