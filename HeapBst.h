/******************************************************************************************
*******************************************************************************************
*************** Online Search with Best-Price and Query-Based Predictions *****************
*******************************************************************************************
*******************************************************************************************/

#include<iostream>
using namespace std;

/* class ConversionInput maintains the search tree
*/

class HeapBST {
private:
	int nn;
	int* a; // main array, storing the tree in level order

	void formTree()
	{
		int counter = 0; // the idnex to be set
		int level_begin = (nn + 1)/2;
		int sub_tree_size = nn + 1;
		for (int level = 0; counter < nn; level ++)
		{	
			int j = level_begin;
			for (int i = 0; i < (1<<level); i++)
			{
				a[counter] = j;
				counter++;
				j += sub_tree_size;
			}
			level_begin /= 2;
			sub_tree_size /= 2;
		}
	}


	// return the index of value x in the array
	int getIndxAux(int x, int root_indx)
	{
		if (root_indx < 0 || root_indx > nn-1)
			return -1;
		if (x == a[root_indx])
			return root_indx;
		if (x < a[root_indx])
			return getIndxAux(x, 2 * root_indx + 1);
		return getIndxAux(x, 2 * root_indx + 2);
	}

	// return the index of value x in the array
	int getIndx(int x)
	{
		/*for (int i = 0; i < nn; i++)
			if (a[i] == x)
				return i;
		return -1;*/
		return getIndxAux(x, 0);
	}

public:

	HeapBST (int n_0)
	{
		nn = 2*n_0 - 1;
		a = new int[nn];
		formTree();
	}

	~HeapBST()
	{
		delete[] a;
	}

	int getRoot()
	{
		return a[0];
	}
	// return the left child of the value x
	int getLeftChild (int x)
	{
		int index = getIndx(x);
		if (index < 0 || index >= nn)
			return -1;
		int i = 2 * index + 1;
		if (i < nn)
			return a[i];
		else 
			return -1;
	}

	int getRightChild(int x)
	{
		int index = getIndx(x);
		if (index < 0 || index >= nn)
			return -1;
		int i = 2 * index + 2;
		if (i < nn)
			return a[i];
		else
			return -1;
	}

	int getParent(int x)
	{
		int index = getIndx(x);
		if (index < 0 || index >= nn)
			return -1;
		if (index == 0)
			return -1;
		else
		{
			int pIndx = (int)((index - 1) / 2);
			return a[pIndx];
		}
	}

	int getLeftmostLeaf(int x)
	{
		int index = getIndx(x);
		if (index < 0 || index >= nn)
		{
			cout << index << " oops x: " << x << "\n";
			return -1;
		}
		while (2 * index + 1 < nn)
			index = 2 * index + 1;
		if (a[index] == -1)
			cout << "error: leftmost " << index << endl;
		return a[index];
	}

	int getRightmostLeaf(int x)
	{
		int index = getIndx(x);
		if (index < 0 || index >= nn)
		{
			cout << index << " oops x: " << x << "\n";
			return -1;
		}
		while (2 * index + 2 < nn)
			index = 2 * index + 2;
		if (a[index] == -1)
			cout << "error: rightmost " << index << endl;
		return a[index];
	}


	int getLevel(int x)
	{
		int depth = 0;
		while (getParent(x) != -1) // while x is not the root
		{
			depth++;
			x = getParent(x);
		}
		return depth;
	}

	int getHeight(int x)
	{
		int left = getLeftChild(x);
		if (left == -1)
			return 0; // no left child == leaf
		int hl = getHeight(left);
		return hl + 1;
	}

	int getSubTreeSize(int x)
	{
		return (1 << (getHeight(x)));
	}

	void display ()
	{
		cout << "Displaying the array:\n";
		for (int i = 0; i < nn; i++)
			cout << a[i] << "  ";
		cout << endl;
		return;
	}

	// returns true if x is an internal node and false otherwise (if x is out of range or a leaf).
	bool isInternal(int x)
	{
		if (getLeftChild(x) != -1)
			return true;
		return false;
	}
};
