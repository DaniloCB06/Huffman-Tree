# Huffman-Tree
This code implements Huffman coding for data compression, including both encoding and decoding functionalities. Huffman coding is a lossless data compression algorithm that assigns variable-length binary codes to characters, with more frequent characters receiving shorter codes.

Here's a breakdown of the code's components:

1. **Data Structures**: 
   - `HeapNodo` represents a node in the Huffman tree, storing a character, its frequency, and pointers to its left and right children.
   - `Heap` is a min-heap used to build the Huffman tree, storing an array of `HeapNodo` pointers.

2. **Heap Operations**: 
   - The `verificaPrioridade()` function maintains the heap property, ensuring that the smallest frequency node is at the root.

3. **Huffman Tree Construction**: 
   - The `constroiHuffman()` function builds the Huffman tree by combining the two nodes with the lowest frequencies into a new parent node until only one node (the root) remains.

4. **Huffman Table Generation**: 
   - The `criaTabelaHuffman()` function recursively traverses the Huffman tree to generate binary codes for each character, storing the codes and their lengths in the `codigoBinarioInt` and `tamanhoBits` arrays.

5. **File Compression**: 
   - The `compacta()` function reads the characters from the input file, converts them into their corresponding Huffman codes, and writes the binary data to a compressed file.

6. **File Decompression**: 
   - The `descompacta()` function reads the compressed binary file and traverses the Huffman tree to reconstruct the original text, which is written to the output file.

7. **Main Function**:
   - The `main()` function reads the input file, calculates character frequencies, builds the Huffman tree, compresses the data, and then decompresses it back to the original form.

Overall, the code provides a complete implementation of Huffman encoding and decoding, effectively compressing and decompressing files using this technique.
