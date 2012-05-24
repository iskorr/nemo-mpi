FILE(REMOVE_RECURSE
  "CMakeFiles/api-doc-man"
  "man3/nemo-c.3"
  "man3/nemo_Network.3"
  "man3/nemo_Simulation.3"
  "man3/nemo_Configuration.3"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/api-doc-man.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
