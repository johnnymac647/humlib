//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Aug  8 12:24:49 PDT 2015
// Last Modified: Thu Jun 13 08:38:17 CEST 2019
// Filename:      HumdrumFileBase.h
// URL:           https://github.com/craigsapp/humlib/blob/master/include/HumdrumFileBase.h
// Syntax:        C++11; humlib
// vim:           syntax=cpp ts=3 noexpandtab nowrap
//
// Description:   Used to store Humdrum text lines from input stream
//                for further parsing.  This class analyzes the basic
//                spine structure after reading a Humdrum file.  The
//                HumdrumFileStructure class continues structural analysis,
//                primarily of rhythm (generated by **kern, **recip and
//                **koto data).
//

#ifndef _HUMDRUMFILEBASE_H_INCLUDED
#define _HUMDRUMFILEBASE_H_INCLUDED

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// USING_URI is defined if you want to be able to download Humdrum data
// from the internet in command-line tools.  JavaScript Humdrum toolkit
// cannot access data from the internet due to the same-origin policy
// in JavaScript unless it is from the computer serving the webpage
// (except under certain circumstances).
#ifdef USING_URI
	#include <sys/types.h>   /* socket, connect */
	#include <sys/socket.h>  /* socket, connect */
	#include <netinet/in.h>  /* htons           */
	#include <netdb.h>       /* gethostbyname   */
	#include <unistd.h>      /* read, write     */
	#include <string.h>      /* memcpy          */
	#include <sstream>
#endif

#include "HumSignifiers.h"
#include "HumdrumLine.h"

namespace hum {

// START_MERGE

// The following options are used for get[Primary]TrackTokens:
// * OPT_PRIMARY    => only extract primary subspine/subtrack.
// * OPT_NOEMPTY    => don't include null tokens in extracted list if all
//                        extracted subspines contains null tokens.
//                        Includes null interpretations and comments as well.
// * OPT_NONULL     => don't include any null tokens in extracted list.
// * OPT_NOINTERP   => don't include interprtation tokens.
// * OPT_NOMANIP    => don't include spine manipulators (*^, *v, *x, *+,
//                        but still keep ** and *0).
// * OPT_NOCOMMENT  => don't include comment tokens.
// * OPT_NOGLOBALS  => don't include global records (global comments, reference
//                        records, and empty lines). In other words, only return
//                        a list of tokens from lines which hasSpines() it true.
// * OPT_NOREST     => don't include **kern rests.
// * OPT_NOTIE      => don't include **kern secondary tied notes.
//
// Compound options:
// * OPT_DATA      (OPT_NOMANIP | OPT_NOCOMMENT | OPT_NOGLOBAL)
//     Only data tokens (including barlines)
// * OPT_ATTACKS   (OPT_DATA | OPT_NOREST | OPT_NOTIE | OPT_NONULL)
//     Only note-attack tokens (when etracting **kern data)
//
#define OPT_PRIMARY   0x001
#define OPT_NOEMPTY   0x002
#define OPT_NONULL    0x004
#define OPT_NOINTERP  0x008
#define OPT_NOMANIP   0x010
#define OPT_NOCOMMENT 0x020
#define OPT_NOGLOBAL  0x040
#define OPT_NOREST    0x080
#define OPT_NOTIE     0x100
#define OPT_DATA      (OPT_NOMANIP | OPT_NOCOMMENT | OPT_NOGLOBAL)
#define OPT_ATTACKS   (OPT_DATA | OPT_NOREST | OPT_NOTIE | OPT_NONULL)


class TokenPair {
	public:
		TokenPair(void) { clear(); }
		~TokenPair() { clear(); }
		void clear(void) {
			first = NULL;
			last  = NULL;
		}
		HTp first;
		HTp last;
};


// HumFileAnalysis: class used to manage analysis states for a Humdrum file.

class HumFileAnalysis {
	public:
		HumFileAnalysis(void) {}
		~HumFileAnalysis() { clear(); }
		void clear(void) {
			m_structure_analyzed = false;
			m_rhythm_analyzed    = false;
			m_strands_analyzed   = false;
			m_slurs_analyzed     = false;
			m_phrases_analyzed   = false;
			m_nulls_analyzed     = false;
			m_strophes_analyzed  = false;

			m_barlines_analyzed  = false;
			m_barlines_different = false;
		}

		// m_structure_analyzed: Used to keep track of whether or not
		// file structure has been analyzed.
		bool m_structure_analyzed = false;

		// m_rhythm_analyzed: Used to keep track of whether or not
		// rhythm structure has been analyzed.
		bool m_rhythm_analyzed = false;

		// m_strands_analyzed: Used to keep track of whether or not
		// file strands have been analyzed.
		bool m_strands_analyzed = false;

		// m_strophes_analyzed: Used to keep track of whether or not
		// file strands have been analyzed.
		bool m_strophes_analyzed = false;

		// m_slurs_analyzed: Used to keep track of whether or not
		// slur endpoints have been linked or not.
		bool m_slurs_analyzed = false;

		// m_phrases_analyzed: Used to keep track of whether or not
		// phrase endpoints have been linked or not.
		bool m_phrases_analyzed = false;

		// m_nulls_analyzed: Used to keep track of wheter or not
		// null tokens have been analyzed yet.
		bool m_nulls_analyzed = false;

		// m_barlines_analyzed: Used to keep track of wheter or not
		// barlines have beena analyzed yet.
		bool m_barlines_analyzed = false;
		// m_barlines_different: Set to true when the file contains
		// any barlines that are not all of the same at the same
		// times.
		bool m_barlines_different = false;
};

bool sortTokenPairsByLineIndex(const TokenPair& a, const TokenPair& b);


class HumdrumFileBase : public HumHash {
	public:
		              HumdrumFileBase          (void);
		              HumdrumFileBase          (HumdrumFileBase& infile);
		              HumdrumFileBase          (const std::string& contents);
		              HumdrumFileBase          (std::istream& contents);
		             ~HumdrumFileBase          ();

		HumdrumFileBase& operator=             (HumdrumFileBase& infile);
		bool          read                     (std::istream& contents);
		bool          read                     (const char* filename);
		bool          read                     (const std::string& filename);
		bool          readCsv                  (std::istream& contents,
		                                        const std::string& separator=",");
		bool          readCsv                  (const char* contents,
		                                        const std::string& separator=",");
		bool          readCsv                  (const std::string& contents,
		                                        const std::string& separator=",");

		bool          readString               (const char* contents);
		bool          readString               (const std::string& contents);
		bool          readStringCsv            (const char* contents,
		                                        const std::string& separator=",");
		bool          readStringCsv            (const std::string& contents,
		                                        const std::string& separator=",");
		bool          isValid                  (void);
		std::string   getParseError            (void) const;
		bool          isQuiet                  (void) const;
		void          setQuietParsing          (void);
		void          setNoisyParsing          (void);
		void          clear                    (void);
		bool          isStructureAnalyzed      (void);
		bool          isRhythmAnalyzed         (void);
		bool          areStrandsAnalyzed       (void);
		bool          areStrophesAnalyzed      (void);

    	template <class TYPE>
		   void       initializeArray          (std::vector<std::vector<TYPE>>& array, TYPE value);

		bool          parse                    (std::istream& contents)
		                                    { return read(contents); }
		bool          parse                    (const char* contents)
		                                    { return readString(contents); }
		bool          parse                    (const std::string& contents)
		                                    { return readString(contents); }
		bool          parseCsv                 (std::istream& contents,
		                                        const std::string& separator = ",")
		                                    { return readCsv(contents); }
		bool          parseCsv                 (const char* contents,
		                                        const std::string& separator = ",")
		                                    { return readStringCsv(contents); }
		bool          parseCsv                 (const std::string& contents,
		                                        const std::string& separator = ",")
		                                    { return readStringCsv(contents); }

		void          setXmlIdPrefix           (const std::string& value);
		std::string   getXmlIdPrefix           (void);
		void          setFilename              (const std::string& filename);
		std::string   getFilename              (void);
		std::string   getFilenameBase          (void);

		void          setSegmentLevel          (int level = 0);
		int           getSegmentLevel          (void);
		std::ostream& printSegmentLabel        (std::ostream& out);
		std::ostream& printNonemptySegmentLabel(std::ostream& out);

		HumdrumLine&  operator[]               (int index);
		HLp           getLine                  (int index);
		int           getLineCount             (void) const;
		HTp           token                    (int lineindex, int fieldindex);
		std::string   token                    (int lineindex, int fieldindex,
		                                        int subtokenindex,
		                                        const std::string& separator = " ");
		int           getMaxTrack              (void) const;
		int           getMaxTracks             (void) const { return getMaxTrack(); }
		int           getTrackCount            (void) const
		                                                { return getMaxTrack(); }
		int           getSpineCount            (void) const
		                                                { return getMaxTrack(); }
		std::vector<int> getMeasureNumbers     (void);
		int           getMeasureNumber         (int line);
		std::ostream& printSpineInfo           (std::ostream& out = std::cout);
		std::ostream& printDataTypeInfo        (std::ostream& out = std::cout);
		std::ostream& printTrackInfo           (std::ostream& out = std::cout);
		std::ostream& printCsv                 (std::ostream& out = std::cout,
		                                        const std::string& separator = ",");
		std::ostream& printFieldNumber         (int fieldnum, std::ostream& out);
		std::ostream& printFieldIndex          (int fieldind, std::ostream& out);
		void          usage                    (const std::string& command);
		void          example                  (void);

		bool          analyzeNonNullDataTokens (void);
		HTp           getTrackStart            (int track) const;
		void          getSpineStopList         (std::vector<HTp>& spinestops);
		HTp           getSpineStart            (int spine) const
		                                       { return getTrackStart(spine+1); }
		void          getSpineStartList        (std::vector<HTp>& spinestarts);
		void          getSpineStartList        (std::vector<HTp>& spinestarts,
		                                        const std::string& exinterp);
		void          getKernSpineStartList    (std::vector<HTp>& spinestarts);
		std::vector<HTp> getKernSpineStartList (void);
		int           getExinterpCount         (const std::string& exinterp);
		void          getSpineStartList        (std::vector<HTp>& spinestarts,
		                                        const std::vector<std::string>& exinterps);
		void          getTrackStartList        (std::vector<HTp>& spinestarts)
		                               { return getSpineStartList(spinestarts); }
		void          getTrackStartList        (std::vector<HTp>& spinestarts,
		                                        const std::string& exinterp)
		                     { return getSpineStartList(spinestarts, exinterp); }
		void          getTrackStartList        (std::vector<HTp>& spinestarts,
		                                        const std::vector<std::string>& exinterps)
		                    { return getSpineStartList(spinestarts, exinterps); }

		int           getTrackEndCount         (int track) const;
		HTp           getTrackEnd              (int track, int subtrack = 0) const;
		void          createLinesFromTokens    (void);
		void          removeExtraTabs          (void);
		void          addExtraTabs             (void);
		std::vector<int> getTrackWidths        (void);
		void          appendLine               (const std::string& line);
		void          appendLine               (HLp line);
		void          push_back                (const std::string& line)
		                                                    { appendLine(line); }
		void          push_back                (HLp line)
		                                                    { appendLine(line); }

		void          insertLine               (int index, const std::string& line);
		void          insertLine               (int index, HLp line);

		HLp           insertNullDataLine                    (HumNum timestamp);
		HLp           insertNullInterpretationLine          (HumNum timestamp);
		HLp           insertNullInterpretationLineAbove     (HumNum timestamp);
		HLp           insertNullInterpretationLineAboveIndex(int index);
		HLp           getLineForInterpretationInsertion     (int index);
		HLp           getLineForInterpretationInsertionAbove(int index);

		void          clearTokenLinkInfo       (void);

		void          deleteLine               (int index);
//		void          adjustMergeSpineLines    (void);

		HLp           back                     (void);
		void          makeBooleanTrackList     (std::vector<bool>& spinelist,
		                                        const std::string& spinestring);
		bool          analyzeBaseFromLines     (void);
		bool          analyzeBaseFromTokens    (void);


		std::vector<HLp> getReferenceRecords(void);
		std::vector<HLp> getGlobalReferenceRecords(void);
		std::vector<HLp> getUniversalReferenceRecords(void);
		std::string getReferenceRecord(const std::string& key);

		// spine analysis functionality:
		void          getTrackSequence         (std::vector<std::vector<HTp> >& sequence,
		                                        HTp starttoken, int options);
		void          getTrackSequence         (std::vector<std::vector<HTp> >& sequence,
		                                        int track, int options);
		void          getPrimaryTrackSequence  (std::vector<HTp>& sequence,
		                                        int track, int options);

		void          getSpineSequence         (std::vector<std::vector<HTp> >& sequence,
		                                        HTp starttoken, int options);
		void          getSpineSequence         (std::vector<std::vector<HTp> >& sequence,
		                                        int spine, int options);
		void          getPrimarySpineSequence  (std::vector<HTp>& sequence,
		                                        int spine, int options);

		void          getTrackSeq              (std::vector<std::vector<HTp> >& sequence,
		                                        HTp starttoken, int options)
		                     { getTrackSequence(sequence, starttoken, options); }
		void          getTrackSeq              (std::vector<std::vector<HTp> >& sequence,
		                                        int track, int options)
		                          { getTrackSequence(sequence, track, options); }
		void          getPrimaryTrackSeq       (std::vector<HTp>& sequence,
		                                        int track, int options)
		                    {getPrimaryTrackSequence(sequence, track, options); }

		// functions defined in HumdrumFileBase-net.cpp:
		static std::string getUriToUrlMapping        (const std::string& uri);
		void          readFromHumdrumUri        (const std::string& humaddress);
		void          readFromJrpUri            (const std::string& jrpaddress);
		void          readFromHttpUri           (const std::string& webaddress);
		static void   readStringFromHttpUri     (std::stringstream& inputdata,
		                                         const std::string& webaddress);

	protected:
		static int    getChunk                  (int socket_id,
		                                         std::stringstream& inputdata,
		                                         char* buffer, int bufsize);
		static int    getFixedDataSize          (int socket_id,
		                                         int datalength,
		                                         std::stringstream& inputdata,
		                                         char* buffer, int bufsize);
		static void   prepare_address           (struct sockaddr_in *address,
		                                         const std::string& hostname,
		                                         unsigned short int port);
		static int    open_network_socket       (const std::string& hostname,
		                                         unsigned short int port);

	protected:
		bool          analyzeTokens             (void);
		bool          analyzeSpines             (void);
		bool          analyzeLinks              (void);
		bool          analyzeTracks             (void);
		bool          adjustSpines              (HumdrumLine& line,
		                                         std::vector<std::string>& datatype,
		                                         std::vector<std::string>& sinfo);
		std::string   getMergedSpineInfo        (std::vector<std::string>& info,
		                                         int starti, int extra);
		bool          stitchLinesTogether       (HumdrumLine& previous,
		                                         HumdrumLine& next);
		void          addToTrackStarts          (HTp token);
		void          addUniqueTokens           (std::vector<HTp>& target,
		                                         std::vector<HTp>& source);
		bool          processNonNullDataTokensForTrackForward(HTp starttoken,
		                                         std::vector<HTp> ptokens);
		bool          processNonNullDataTokensForTrackBackward(HTp starttoken,
		                                         std::vector<HTp> ptokens);
		bool          setParseError             (std::stringstream& err);
		bool          setParseError             (const std::string& err);
		bool          setParseError             (const char* format, ...);
		bool          analyzeLines              (void);
//		void          fixMerges                 (int linei);

	protected:

		// m_lines: an array representing lines from the input file.
		// The contents of lines must be deallocated when deconstructing object.
		std::vector<HLp> m_lines;

		// m_filename: name of the file which was loaded.
		std::string m_filename;

		// m_segementlevel: segment level (e.g., work/movement)
		int m_segmentlevel;

		// m_trackstarts: list of addresses of the exclusive interpreations
		// in the file.  The first element in the list is reserved, so the
		// number of tracks (primary spines) is equal to one less than the
		// size of this list.
		std::vector<HTp> m_trackstarts;

		// m_trackends: list of the addresses of the spine terminators in the
		// file. It is possible that spines can split and their subspines do not
		// merge before termination; therefore, the ends are stored in
		// a 2d array. The first dimension is the track number, and the second
		// dimension is the list of terminators.
		std::vector<std::vector<HTp> > m_trackends;

		// m_barlines: list of barlines in the data.  If the first measures is
		// a pickup measure, then the first entry will not point to the first
		// starting exclusive interpretation line rather than to a barline.
		std::vector<HLp> m_barlines;
		// Maybe also add "measures" which are complete metrical cycles.

		// m_ticksperquarternote: this is the number of tick
		int m_ticksperquarternote;

		// m_idprefix: an XML id prefix used to avoid id collisions when
		// including multiple HumdrumFile XML in a single group.
		std::string m_idprefix;

		// m_strands1d: one-dimensional list of spine strands.
		std::vector<TokenPair> m_strand1d;

		// m_strands2d: two-dimensional list of spine strands.
		std::vector<std::vector<TokenPair> > m_strand2d;

		// m_strophes1d: one-dimensional list of all *strophe/*Xstrophe pairs.
		std::vector<TokenPair> m_strophes1d;

		// m_strophes2d: two-dimensional list of all *strophe/*Xstrophe pairs.
		std::vector<std::vector<TokenPair> > m_strophes2d;

		// m_quietParse: Set to true if error messages should not be
		// printed to the console when reading.
		bool m_quietParse;

		// m_parseError: Set to true if a read is successful.
		std::string m_parseError;

		// m_displayError: Used to print error message only once.
		bool m_displayError;

		// m_signifiers: Used to keep track of !!!RDF records.
		HumSignifiers m_signifiers;

		// m_analysis: Used to keep track of analysis states for the file.
		HumFileAnalysis m_analyses;

	public:
		// Dummy functions to allow the HumdrumFile class's inheritance
		// to be shifted between HumdrumFileContent (the top-level default),
		// HumdrumFileStructure (mid-level interface), or HumdrumFileBase
		// (low-level interface).

		//
		// HumdrumFileStructure public functions:
		//
		bool readNoRhythm      (std::istream& infile) { return read(infile); };
		bool readNoRhythm      (const char*   filename) {return read(filename);};
		bool readNoRhythm      (const std::string& filename) {return read(filename);};
		bool readStringNoRhythm(const char*   contents) {return read(contents);};
		bool readStringNoRhythm(const std::string& contents) {return read(contents);};
		HumNum       getScoreDuration           (void) const { return 0; };
		std::ostream& printDurationInfo         (std::ostream& out=std::cout) {return out;};
		int          tpq                        (void) { return 0; }
		int          getBarlineCount            (void) const { return 0; }
		HLp          getBarline                 (int index) const { return NULL;};
		HumNum       getBarlineDuration         (int index) const { return 0; };
		HumNum       getBarlineDurationFromStart(int index) const { return 0; };
		HumNum       getBarlineDurationToEnd    (int index) const { return 0; };

		// HumdrumFileContent public functions:
		// to be added later

};

std::ostream& operator<<(std::ostream& out, HumdrumFileBase& infile);


// END_MERGE

} // end namespace hum

#endif /* _HUMDRUMFILEBASE_H_INCLUDED */



