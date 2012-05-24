FILE(REMOVE_RECURSE
  "CMakeFiles/api-doc-latex"
  "manual.pdf"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/api-doc-latex.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
