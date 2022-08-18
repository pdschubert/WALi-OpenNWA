# In den Blättern des Verzeichnisbaums darf man dann ruhig 'GLOB_RECURSE'
# verwenden. Alternativ kann man hier die Quelldateien auch von Hand
# in einer Liste angeben.
file(GLOB_RECURSE WALI_SRC *.h *.cpp)
FOREACH(item ${WALI_SRC})
	IF(${item} MATCHES "wpds/LinkedTrans.cpp" OR
			${item} MATCHES "graph/Experimental.cpp" OR
			${item} MATCHES "details/PriorityWorklist.multiset.cpp" OR
			${item} MATCHES "details/PriorityWorklist.boost_heap.cpp")
		LIST(REMOVE_ITEM WALI_SRC ${item})
	ENDIF(${item} MATCHES "wpds/LinkedTrans.cpp" OR
			${item} MATCHES "graph/Experimental.cpp" OR
			${item} MATCHES "details/PriorityWorklist.multiset.cpp" OR
			${item} MATCHES "details/PriorityWorklist.boost_heap.cpp")
ENDFOREACH(item)


add_library(wali # Name der Bibliothek
		${WALI_SRC} # Quellcode der zur dieser Bibliothek gehören soll
)

set(LLVM_LINK_COMPONENTS
  Support
  Core
)
if(USE_LLVM_FAT_LIB)
  llvm_config(wali USE_SHARED ${LLVM_LINK_COMPONENTS})
else()
  llvm_config(wali ${LLVM_LINK_COMPONENTS})
endif()

# Falls nötig und die Bibliothek selbst Abhangigkeiten besitzt,
# kann man dieser hiermit dazu linken. Man sollte allerdings
# soweit es geht darauf verzichten, da man ansonsten schnell in
# der 'dependency hell' landet.
# target_link_libraries(test_all
# )

set_target_properties(wali
	PROPERTIES
	LINKER_LANGUAGE CXX # Linkersprache (bei CXX also C++ findet z.B. name mangling statt)
	PREFIX "lib" # Der typische 'lib' Prefix einer Bibliothek auf Unix-Systemen
)

install(TARGETS wali ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
