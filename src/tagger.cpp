#include "tagger.h"

#include <id3v2tag.h>
#include <mpegfile.h>
#include <textidentificationframe.h>

#include <boost/log/trivial.hpp>

const std::string RATING_DESCRIPTION = "NIGHTINGALE-RATING";

void setRating(const boost::filesystem::path& target, int rating) {
	BOOST_LOG_TRIVIAL(debug) << target << ": Setting rating " << rating;

	auto ratingText = std::to_string(rating);
	TagLib::MPEG::File file(target.native().c_str());
	if (auto tag = file.ID3v2Tag()) {
		if (auto frame = TagLib::ID3v2::UserTextIdentificationFrame::find(tag, RATING_DESCRIPTION)) {
			if (frame->fieldList()[1] != ratingText) {
				BOOST_LOG_TRIVIAL(info) << "Updating rating to " << rating;
				frame->setText(ratingText);
				file.save();
			}
			else {
				BOOST_LOG_TRIVIAL(debug) << "Not touching rating " << rating;
			}
		}
		else {
			BOOST_LOG_TRIVIAL(info) << "Setting new rating to " << rating;
			auto newFrame = new TagLib::ID3v2::UserTextIdentificationFrame();
			newFrame->setDescription(RATING_DESCRIPTION);
			newFrame->setText(ratingText);
			tag->addFrame(newFrame);
			file.save();
		}
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << target << ": No ID3v2 tag found";
	}
}
