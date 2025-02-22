#include <filesystem>

#include <seqan3/core/debug_stream.hpp>
#include <bio/var_io/reader.hpp>

#include "../../unit/format/vcf_data.hpp"

int main()
{
//================= PRE ==========================
    {
        std::ofstream os{"example.vcf", std::ios::binary};
        os << example_from_spec_header;
        os << example_from_spec_records;
    }

    std::ifstream in{"example.vcf"};
    std::cin.rdbuf(in.rdbuf()); // rewire stdin

//================= SNIPPETS ======================

{
//![field_types_deep]
// this results in the records becoming "copyable"
bio::var_io::reader_options options{ .field_types = bio::var_io::field_types<bio::ownership::deep> };

// read the entire file, copy all records into a vector; immediately closes file again
std::vector records = bio::var_io::reader{"example.vcf", options} | seqan3::ranges::to<std::vector>();

/* do something else */

// process the records later-on
for (auto & rec : records)
{
    seqan3::debug_stream << rec.chrom() << ':'
                         << rec.pos()   << ':'
                         << rec.ref()   << ':'
                         << rec.alt()   << '\n';
}
//![field_types_deep]
}

{
//![field_types_expert]
bio::var_io::reader_options options{
    .field_ids   = bio::vtag<bio::field::chrom, bio::field::pos>,
    .field_types = bio::ttag<std::string_view, int32_t> };

bio::var_io::reader reader{"example.vcf", options};

for (auto & rec : reader)
{
    seqan3::debug_stream << rec.chrom() << ':'
                         << rec.pos()   << '\n';
    // record does not have any other members!
}
//![field_types_expert]
}

//================= POST ==========================
    std::filesystem::remove("example.vcf");
}
