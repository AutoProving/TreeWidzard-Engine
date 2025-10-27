#!/bin/bash

# TreeWidzard Repository Cleanup Script
# Removes old files, temporary outputs, and organizes the repository

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

echo "=== TreeWidzard Repository Cleanup ==="
echo "Working directory: $PROJECT_ROOT"
echo ""

# Create backup before cleanup
echo "Creating backup of current state..."
tar -czf "backup-$(date +%Y%m%d-%H%M%S).tar.gz" \
    --exclude='.git' \
    --exclude='build' \
    --exclude='*.tar.gz' \
    .

echo ""
echo "1. Removing old and temporary files..."

# Remove old CMake files
rm -f CMakeLists_OLD.txt
echo "   ✓ Removed CMakeLists_OLD.txt"

# Remove duplicate license
rm -f "LICENSE copy"
echo "   ✓ Removed duplicate LICENSE file"

# Remove old parser files
rm -rf Parser/old_files/
echo "   ✓ Removed Parser/old_files/"

# Remove build artifacts that shouldn't be in repo
find . -name "*.o" -type f -delete
find . -name "*.so" -type f -delete
find . -name "*.dylib" -type f -delete
find . -name "*.a" -type f -delete
echo "   ✓ Removed build artifacts (.o, .so, .dylib, .a files)"

# Remove backup files from editors
find . -name "*~" -type f -delete
find . -name "*.bak" -type f -delete
find . -name "*.orig" -type f -delete
echo "   ✓ Removed editor backup files"

# Remove generated output files from examples
find examples/conjectures/ -name "*CounterExample*" -type f -delete
find examples/conjectures/ -name "*RunTree*" -type f -delete
echo "   ✓ Removed generated example output files"

echo ""
echo "2. Organizing documentation..."

# Keep only essential documentation
ESSENTIAL_DOCS=(
    "README.md"
    "ARCHITECTURE.md" 
    "CONJECTURE_FORMAT.md"
    "USER_GUIDE.md"
    "PERFORMANCE_OPTIMIZATION.md"
)

cd docs/
for doc in *.md; do
    if [[ ! " ${ESSENTIAL_DOCS[@]} " =~ " ${doc} " ]]; then
        echo "   - Moving $doc to archive/"
        mkdir -p archive/
        mv "$doc" archive/
    fi
done
cd ..

echo "   ✓ Organized documentation (moved non-essential docs to archive/)"

echo ""
echo "3. Cleaning up temporary test outputs..."

# Remove test output files
find test_conjectures/ -name "*.txt" -size +100k -delete 2>/dev/null || true
echo "   ✓ Removed large test output files"

echo ""
echo "4. Updating .gitignore..."

# Ensure .gitignore covers what we need
cat >> .gitignore << 'EOF'

# Build artifacts
*.o
*.so
*.dylib
*.a
build/
Build/
cmake-build-*/

# Generated outputs
*CounterExample*
*RunTree*
*_tree_width_*
*_path_width_*

# Editor files
*~
*.bak
*.orig
.vscode/
.idea/

# Backup files
backup-*.tar.gz

# Documentation archives
docs/archive/

# Performance reports
performance_report.txt
memory_usage.log
*.data

EOF

echo "   ✓ Updated .gitignore"

echo ""
echo "5. Repository statistics:"
echo "   Files: $(find . -type f | wc -l)"
echo "   Directories: $(find . -type d | wc -l)"
echo "   Total size: $(du -sh . | cut -f1)"

echo ""
echo "=== Cleanup Complete ==="
echo ""
echo "Removed:"
echo "  • Old CMake and license files"
echo "  • Parser old_files directory"
echo "  • Build artifacts and editor backups"
echo "  • Generated example outputs"
echo "  • Large temporary test files"
echo ""
echo "Organized:"
echo "  • Documentation (essential docs kept, others archived)"
echo "  • Updated .gitignore for better exclusions"
echo ""
echo "Ready for commit!"