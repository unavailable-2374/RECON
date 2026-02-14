# RECON - De Novo Repeat Family Identification (Performance-Optimized Fork)

> **This is a performance-optimized fork of RECON v1.05, originally developed by Zhirong Bao and Sean Eddy.**
>
> **Original source:** <http://eddylab.org/software/recon/>
>
> **Original paper:** Bao Z. and Eddy S.R. (2002) "Automated de novo Identification of Repeat Sequence Families in Sequenced Genomes." *Genome Research*, 12:1269-1276.
>
> **Original copyright:** Copyright (C) 2001 Washington University School of Medicine. Licensed under GPLv2.

## What's Changed in This Fork

This fork applies algorithmic, I/O, and systems-level performance optimizations to the original RECON codebase **without changing any scientific output** (results are byte-identical to the original on the included demo dataset). The optimizations target large genome datasets where the original code can take days to run.

### Optimization Summary

| Category | Change | Estimated Speedup |
|---|---|---|
| **Algorithm fixes** | O(n^2) → O(n) sibling-list insertion in `consis_tree_build`, O(1) image removal via doubly-linked list + backpointers, dynamic `all_ele` array resizing | 2-10x |
| **I/O optimization** | `mmap`-based MSP file access (eliminates `rewind`), `unlink()`/`fwrite` instead of `system("rm")`/`system("cp")`, 64KB I/O buffers, reduced `fflush` calls | 2-5x |
| **Data structures** | BST periodic rebalancing for image and edge trees, memory pool allocator for high-frequency structs (MSP_t, IMG_TREE_t, EDGE_TREE_t, EDGE_t, IMG_DATA_t) | 1.5-3x |
| **Compiler & sort** | `-O2 -march=native`, GNU `sort --parallel=$(nproc)` | 2-4x (sort stage) |

### Bug Fixes

- Fixed `%d` vs `%ld` format string mismatch for `long` fields in `sscanf`/`fprintf` (ele.h) — the original code relied on undefined behavior on 64-bit systems.
- Zero-initialized `MSP_t` structs via `memset` to prevent uninitialized memory reads.

## Installation

```bash
cd src
make
make install
```

Then edit `scripts/recon.pl` line 3 to set `$path` to the absolute path of the `bin/` directory.

## Usage

```bash
# Step 1: Convert BLAST output to MSP format
scripts/MSPCollect.pl BLAST_output_file > output.msps

# Step 2: Prepare sorted sequence name list (with count as first line)
grep ">" sequences.fa | sort > seqnames
# Manually prepend the count as the first line

# Step 3: Run
scripts/recon.pl seqnames output.msps [num_sort_sections]
```

See `00README` for full details.

### Demo

```bash
cd Demos/output
../../scripts/recon.pl ../input/seqnames ../input/elegans.msps 1
```

## Output

Final results in `summary/`:
- `eles` — One line per element: `family_index element_index strand sequence_name start end`
- `families` — One line per family: `family_index copy_count`

## License

GPLv2, same as the original. See `LICENSE` and `COPYRIGHT`.

## Acknowledgments

All credit for the RECON algorithm and original implementation goes to **Zhirong Bao** and **Sean Eddy** at the Eddy Lab. This fork only modifies performance characteristics without altering the scientific methodology.

- Original website: <http://eddylab.org/software/recon/>
- Original paper: Bao Z. and Eddy S.R. (2002) *Genome Research*, 12:1269-1276.
