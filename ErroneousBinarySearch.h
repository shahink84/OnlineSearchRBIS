/******************************************************************************************
*******************************************************************************************
*************** Online Search with Best-Price and Query-Based Predictions *****************
*******************************************************************************************
*******************************************************************************************/
#include <stack>
#include "HeapBst.h"

/* class ErroneousBinarySearch implements RBIS
*/
class ErroneousBinarySearch
{
private:
	int k; // number of queries (fixed in our experiments)
	int n; // number of leaves in the tree
	
    // check whether x<=node_value; return th eright answer if and only if answers[q] = true
    bool get_answer(int x, int node_value, bool* answers, int q)
    {
        bool right_answer = (x <= node_value);
        bool answer = right_answer;
        if (answers[q] == false)
            answer = !answer;
        return answer;
    }

public:
    HeapBST* T; // used for accessing leftmost/rightmost leaves when setting alg cost in Conversioninput

	ErroneousBinarySearch(int k0, int n0) {
		k = k0;
		n = n0;
		T = new HeapBST(n); 
	}

    ~ErroneousBinarySearch()
    {
        delete T;
    }
    
    /* search for x in T, given array of answers. Continue search until k queries are used.
    The algorithm is H-aware, and stops double-checking when the number of detected errors become H.
    The output is a node of the tree that is guaranteed to contain x in its subtree */
    int bs_limited_query(int x, bool* answers, int H, bool printLog)
    {
        int root = T->getRoot();
        int cur_node = root;
        stack <int>* prev_answers = new stack<int>[2];
        int remaining_errors = H;
        int q;
        for (q = 0; q < k && cur_node != -1 && remaining_errors > 0;) // q is the index of the query; continue untill all queries are used or you reach a leaf
        {
            if (printLog)
                cout << "q is: " << q << "  cur_Node is: " << cur_node << endl;
            bool cur_answer = get_answer(x, cur_node, answers, q);
            q++;

            int anc_node = -1;
            if (prev_answers[!cur_answer].empty() == false)
                anc_node = prev_answers[!cur_answer].top(); // the ancestor with opposite answer to the current node. 
            if (printLog)
                cout << "anc_node: " << anc_node << endl;
    
            bool error_found = false;
            bool contradictig_answer = true;

            while (contradictig_answer && anc_node != -1 && cur_node != anc_node && q < k)
            {
                contradictig_answer = (get_answer(x, anc_node, answers, q) == cur_answer);
                q++;
                if (contradictig_answer && cur_node != root)
                {
                    // the answer to the new query for ancestor contradicts the previous answer to same query.
                    if (printLog)
                        cout << "backtrack" << cur_node << "  " << anc_node << endl;
                    cur_node = T->getParent(cur_node); // the new answer on anc_node contradits the previous one; backtrack
                    if (printLog)
                        cout << "\tcur_node is now: " << cur_node << "remaining errors is: " << remaining_errors << endl;
                    error_found = true;
                }
            }
            remaining_errors -= error_found; // remaining errors; if an error is found, decrement.
            if (!error_found && (cur_node == root || cur_node != anc_node) && q < k)
            {
                // if error is found you should not try getting deeper in the tree
                if (T->isInternal(cur_node))
                {
                    // go deeper in the tree without double-checking
                    prev_answers[cur_answer].push(cur_node);
                    if (cur_answer == true) // x <= cure_node
                        cur_node = T->getLeftChild(cur_node); // left/left
                    else
                        cur_node = T->getRightChild(cur_node);// right/right
                }
                else
                    break; // we reached a leaf; end search
            }
            else
            {
                if (cur_node == anc_node && q < k) // we backtracked all the way to the previous answer. That answer should be poped.
                    prev_answers[!cur_answer].pop();
            }
        }

        for (int i = 0; i < remaining_errors && cur_node != root; i++)
            cur_node = T->getParent(cur_node);

        if (printLog)
            cout << "current node before messing: " << cur_node << " and q is: " << q << " and H was: " << H << endl;
        while (remaining_errors == 0 && T->isInternal(cur_node) && q < k)
        {  
            // go all the way to the leaf; you can trust now since all answers are correct
            // go deeper in the tree without double-checking
            bool cur_answer = get_answer(x, cur_node, answers, q);
            if (cur_answer == true) // x <= cure_node
                cur_node = T->getLeftChild(cur_node); // left/left
            else
                cur_node = T->getRightChild(cur_node);// right/right
            q++;
        }//*/

        if (printLog)
            cout << "and after messing: " << cur_node << endl;

        return cur_node;
    }

    // search for a leaf x that is selected randomly, return x - result(search); used for testing
    int test_random_search(bool* answers, int H, bool printLog)
    {
        int x = rand() % n; // a number in [0,n-1]
        // leaves are odd in the range [1,2n-1]
        x = 2 * x + 1;
        int res = bs_limited_query(x, answers, H, printLog);
        res = T->getLeftmostLeaf(res);
        if (res > x)
        {
            cout << "ERROR: mistake in search\nx:" << x << "\t res: " << res << endl << "answers:" << endl;
            for (int q = 0; q < k; q++)
                cout << answers[q] << "\t";
            cout << endl;
            bs_limited_query(x, answers, H, 1);
        }
        return x - res;
    }
};