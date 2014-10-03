;this method checks if 'a' is a variable as defined in the problem
(defun is-var(a)
  (and (symbolp a) (and (not (null a)) (and (> (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\?)))))

;this method checks if 'a' is of the form '!X' where X could be any sequence of characters
(defun is-exclamation(a)
  (and (symbolp a) (and (not (null a)) (and (> (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\!)))))

;this method checks if 'a' is of the form '<X' where X could be any sequence of characters 
 (defun is-ltvar(a)
  (and (symbolp a) (and (not (null a)) (and (> (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\<)))))
 
 ;this method checks if 'a' is of the form '>X' where X could be any sequence of characters
 (defun is-gtvar(a)
  (and (symbolp a) (and (not (null a)) (and (> (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\>)))))

;this method checks if 'a' is '?' (probably didn't need a separate method for this but it improves the readability of code)
(defun is-q-mark(a)
  (and (symbolp a) (and (not (null a)) (and (= (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\?)))))

;this method checks if 'a' is '*' (probably didn't need a separate method for this but it improves the readability of code)
(defun is-kleene-star(a)
  (and (symbolp a) (and (not (null a)) (and (= (length (symbol-name a)) 1) (equal (char (symbol-name a) 0) #\*)))))

;this method checks if 'a' is a list and its first element is the character '&'
(defun is-amp-exp(a)
  (and (listp a) (and (not (null a)) (and (> (length a) 1) (and (symbolp (first a)) (equal (char (symbol-name (first a)) 0) #\&))))))
 
;this method checks that whether all elements of 'the list a' equal '*'
(defun is-only-*-elements (a)
  (cond 
    ((null a) t)
	((is-kleene-star (car a)) (is-only-*-elements (cdr a)))
	(t nil)))
 
;returns the value of the entity to which elm is bound from the association list alist
(defun get-binding (elm alist)
   (second (assoc elm alist))) 

;returns the variable(with question mark) associated with the symbol following a '!' or '<' or '>'
(defun get-var (a)
   (intern (string-upcase (concatenate 'string "?" (string-left-trim "!<>" (symbol-name a))))))

;this method merges 2 variables, each of which can hold one of three values: 't', 'nil' or an 'association list'
;there is an and condition between these two variables, i.e., the patterns match only if none of these are nil 
(defun merge-lists (list1 list2)
  (cond 
    ((or (null list1) (null list2)) nil)
	((and (listp list1) (listp list2)) (remove-duplicates (append list1 list2) :test #'equal))
	((listp list1) list1)
	((listp list2) list2)
	(t t)))


;this method merges 3 variables, each of which can hold one of three values: 't', 'nil' or an 'association list'
;there is an or condition between these two variables, i.e., the patterns match if any of these is 'true' or 'association list'
(defun merge-*-results (alist1 alist2 alist3)
  (cond 
    ((and (null alist1) (and (null alist2) (null alist3))) nil)
	((and (null alist2) (null alist3)) 
	  (cond ((listp alist1) alist1) (t t)))
	((and (null alist1) (null alist3))
	  (cond ((listp alist2) alist2) (t t)))
	((and (null alist1) (null alist2))
	  (cond ((listp alist3) alist3) (t t)))
	((null alist1)
	  (cond
	   ((and (listp alist2) (listp alist3)) (intersect alist2 alist3))
	   ((listp alist2) alist2)
	   ((listp alist3) alist3)
	   (t t)))
	((null alist2)
	  (cond
	   ((and (listp alist1) (listp alist3)) (intersect alist1 alist3))
	   ((listp alist1) alist1)
	   ((listp alist3) alist3)
	   (t t)))
	((null alist3)
	  (cond
	   ((and (listp alist1) (listp alist2)) (intersect alist1 alist2))
	   ((listp alist1) alist1)
	   ((listp alist2) alist2)
	   (t t)))
	(t 
	  (cond
	   ((and (listp alist3) (and (listp alist1) (listp alist2))) (intersect alist1 (intersect alist2 alist3)))
	   ((and (listp alist1) (listp alist2)) (intersect alist1 alist2))
	   ((and (listp alist2) (listp alist3)) (intersect alist2 alist3))
	   ((and (listp alist1) (listp alist3)) (intersect alist1 alist3))
	   ((listp alist1) alist1)
	   ((listp alist2) alist2)
	   ((listp alist3) alist3)
	   (t t)))))

;this method merges 2 lists and removes duplicate entries
;will be called with lists only (no need to check for listp alist)	   
(defun intersect (alist1 alist2)
  (cond
    ((equal alist1 alist2) alist1)
	(t (remove-duplicates (append alist1 alist2) :test #'equal))))

;this method recursively calculates the boolean result of an 'AND' expression appearing in the pattern
;by matching each condition in the expression with the corresponding entry in the data
(defun and-exp (expr data alist)
  (if (not (null expr))
    (let ((currcond (car expr)))
      (cond 
	     ((is-var currcond)
           (cond
 		     ((eq (get-binding currcond alist) data)
			   (and-exp (cdr expr) data alist))
			 (t nil)))
         ((is-exclamation currcond) 
		   (cond
 		     ((not (eq (get-binding (get-var currcond) alist) data))
			   (and-exp (cdr expr) data alist))
			 (t nil)))
         ((is-ltvar currcond) 
		   (cond
 		     ((< data (get-binding (get-var currcond) alist))
			   (and-exp (cdr expr) data alist))
			 (t nil)))
         ((is-gtvar currcond) 
           (cond
 		     ((> data (get-binding (get-var currcond) alist))
			   (and-exp (cdr expr) data alist))
			 (t nil)))
		 (t nil)))
	t))
	
;this is the primary function which recurses through the pattern(list1) and data(list2) and maintains an association list(alist) of variable bindings  
(defun pattern-matcher (list1 list2 alist) 
(if (and (not (null list1)) (not (null list2)))
 (let ((a (car list1)) (b (car list2))) 
 (cond
  ((and (listp a) (listp b))
    (merge-lists (pattern-matcher a b alist) (pattern-matcher (cdr list1)(cdr list2) alist))) 
  ((is-kleene-star a)
    (merge-*-results (pattern-matcher list1 (cdr list2) alist) (pattern-matcher (cdr list1) list2 alist) (pattern-matcher (cdr list1) (cdr list2) alist)))
  ((is-var a)
    (cond
	  ((assoc a alist)
	     (cond 
		   ((equal (get-binding a alist) b) (pattern-matcher (cdr list1)(cdr list2) alist))
		   (t nil)))
	  (t (pattern-matcher (cdr list1) (cdr list2) (cons (list a b) alist)))))
  ((is-exclamation a) 
    (and (not (eq (get-binding (get-var a) alist) b)) (pattern-matcher (cdr list1) (cdr list2) alist)))
  ((is-ltvar a) 
    (and (< b (get-binding (get-var a) alist)) (pattern-matcher (cdr list1) (cdr list2) alist)))
  ((is-gtvar a) 
    (and (> b (get-binding (get-var a) alist)) (pattern-matcher (cdr list1) (cdr list2) alist)))
  ((is-amp-exp a) 
    (and (and-exp (cdr a) b alist) (pattern-matcher (cdr list1) (cdr list2) alist)))
  (t
    (and (or (is-q-mark a) (eq a b)) (pattern-matcher (cdr list1) (cdr list2) alist)))))
 (if (or (and (null list1) (null list2)) (and (not (null list1)) (is-only-*-elements list1)))
;the above condition checks that either list1 and list2 both are null or the only elements left in the pattern are '*'
;in both these cases the lists match and we should return alist if its not empty and true otherwise
  (cond ((not (null alist)) alist) (t t))
	nil)))

;this is the driver method of the pattern-matcher
;this method excuses the user from passing an empty assoc list to pattern-matcher while making each call
(defun pattern-match-wrapper (pattern data)
   (pattern-matcher pattern data nil))