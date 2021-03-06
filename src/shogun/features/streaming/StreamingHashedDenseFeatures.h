/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Evangelos Anagnostopoulos, Yuyu Zhang, Bjoern Esser, Viktor Gal
 */

#ifndef _STREAMING_HASHED_DENSEFEATURES__H__
#define _STREAMING_HASHED_DENSEFEATURES__H__

#include <shogun/lib/config.h>

#include <shogun/features/DenseFeatures.h>
#include <shogun/features/streaming/StreamingDotFeatures.h>
#include <shogun/io/streaming/InputParser.h>

namespace shogun
{
class StreamingDotFeatures;

/** @brief This class acts as an alternative to the StreamingDenseFeatures class
 * and their difference is that the current example in this class is hashed into
 * a smaller dimension dim.
 *
 * The current example is stored as a combination of current_vector
 * and current_label. Call get_next_example() followed by get_current_vector()
 * to iterate through the stream.
 */
template <class ST> class StreamingHashedDenseFeatures : public StreamingDotFeatures
{
public:
	/** Constructor */
	StreamingHashedDenseFeatures();

	/**
	 * Constructor with input information passed.
	 *
	 * @param file CStreamingFile to take input from.
	 * @param is_labelled Whether examples are labelled or not.
	 * @param size Number of examples to be held in the parser's "ring".
	 * @param d the dimensionality of the target feature space
	 * @param use_quadr whether to use quadratic features or not
	 * @param keep_lin_terms whether to maintain the linear terms in the computations
	 */
	StreamingHashedDenseFeatures(const std::shared_ptr<StreamingFile>& file, bool is_labelled, int32_t size,
			 int32_t d = 512, bool use_quadr = false, bool keep_lin_terms = true);

	/**
	 * Constructor taking a DotFeatures object and optionally,
	 * labels, as args.
	 *
	 * The derived class should implement it so that the
	 * Streaming*Features class uses the DotFeatures object as the
	 * input, getting examples one by one from the DotFeatures
	 * object (and labels, if applicable).
	 *
	 * @param dot_features DotFeatures object
	 * @param d the dimensionality of the target feature space
	 * @param use_quadr whether to use quadratic features or not
	 * @param keep_lin_terms whether to maintain the linear terms in the computations
	 * @param lab labels (optional)
	 */
	StreamingHashedDenseFeatures(std::shared_ptr<DenseFeatures<ST>> dot_features, int32_t d = 512,
			bool use_quadr = false, bool keep_lin_terms = true, float64_t* lab = NULL);

	/** Destructor */
	~StreamingHashedDenseFeatures() override;

	/** compute dot product between vectors of two
	 * StreamingDotFeatures objects.
	 *
	 * @param df StreamingDotFeatures (of same kind) to compute
	 * dot product with
	 */
	float32_t dot(std::shared_ptr<StreamingDotFeatures> df) override;

	/** compute dot product between current vector and a dense vector
	 *
	 * @param vec2 real valued vector
	 * @param vec2_len length of vector
	 */
	float32_t dense_dot(const float32_t* vec2, int32_t vec2_len) override;

	/** add current vector multiplied with alpha to dense vector, 'vec'
	 *
	 * @param alpha scalar alpha
	 * @param vec2 real valued vector to add to
	 * @param vec2_len length of vector
	 * @param abs_val if true add the absolute value
	 */
	void add_to_dense_vec(float32_t alpha, float32_t* vec2,
			int32_t vec2_len, bool abs_val = false) override;

	/** obtain the dimensionality of the feature space
	 *
	 * (not mix this up with the dimensionality of the input space, usually
	 * obtained via get_num_features())
	 *
	 * @return dimensionality
	 */
	int32_t get_dim_feature_space() const override;

	/**
	 * Return the name.
	 *
	 * @return the name of the class
	 */
	const char* get_name() const override;

	/**
	 * Return the number of vectors stored in this object.
	 *
	 * @return 1 if current_vector exists, else 0.
	 */
	int32_t get_num_vectors() const override;

	/**
	 * Sets the read function (in case the examples are
	 * unlabelled) to get_*_vector() from CStreamingFile.
	 *
	 * The exact function depends on type ST.
	 *
	 * The parser uses the function set by this while reading
	 * unlabelled examples.
	 */
	void set_vector_reader() override;

	/**
	 * Sets the read function (in case the examples are labelled)
	 * to get_*_vector_and_label from CStreamingFile.
	 *
	 * The exact function depends on type ST.
	 *
	 * The parser uses the function set by this while reading
	 * labelled examples.
	 */
	void set_vector_and_label_reader() override;

	/**
	 * Return the feature type, depending on ST.
	 *
	 * @return Feature type as EFeatureType
	 */
	EFeatureType get_feature_type() const override;

	/**
	 * Return the feature class
	 *
	 * @return C_STREAMING_DENSE
	 */
	EFeatureClass get_feature_class() const override;

	/**
	 * Start the parser.
	 * It stores parsed examples from the input in a separate thread.
	 */
	void start_parser() override;

	/**
	 * End the parser. Wait for the parsing thread to complete.
	 */
	void end_parser() override;

	/**
	 * Return the label of the current example.
	 *
	 * Raise an error if the input has been specified as unlabelled.
	 *
	 * @return Label (if labelled example)
	 */
	float64_t get_label() override;

	/**
	 * Indicate to the parser that it must fetch the next example.
	 *
	 * @return true on success, false on failure (i.e., no more examples).
	 */
	bool get_next_example() override;

	/**
	 * Indicate that processing of the current example is done.
	 * The parser then considers it safe to dispose of that example
	 * and replace it with another one.
	 */
	void release_example() override;

	/**
	 * Get the number of features in the current example.
	 *
	 * @return number of features in current example
	 */
	int32_t get_num_features() override;

	/** Get the current example
	 *
	 * @return a SGSparseVector representing the hashed version of the string last read
	 */
	SGSparseVector<ST> get_vector();

private:
	void init(std::shared_ptr<StreamingFile> file, bool is_labelled, int32_t size,
		int32_t d, bool use_quadr, bool keep_lin_terms);

protected:

	/** dimensionality of new feature space */
	int32_t dim;

	/** Current example */
	SGSparseVector<ST> current_vector;

	/** The parser */
	InputParser<ST> parser;

	/** The current example's label */
	float64_t current_label;

	/** use quadratic feature or not */
	bool use_quadratic;

	/** keep linear terms or not */
	bool keep_linear_terms;
};
}

#endif // _STREAMING_HASHED_DENSEFEATURES__H__
