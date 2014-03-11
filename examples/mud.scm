; ===== SPEEDWALK =====
(define speedwalk-check-regex (make-regexp "^([0-9]*[nsewud])+$"))
(define speedwalk-part-regex (make-regexp "([0-9]*)([nsewud])"))

; Function for finding all matches against a regex.
(define all-matches
  (lambda (regex str)
    (let loop ((match (regexp-exec regex str)) (matches '()))
      (if match
	  (loop (regexp-exec regex str (match:end match)) (cons match matches))
	  (reverse matches)))))

; Function for converting a speedwalk part to individual commands
; (e.g. "4n" -> ("n" "n" "n" "n"))
(define parse-speedwalk-part
  (lambda (part)
    (let ((num (string->number (match:substring part 1)))
	  (dir (match:substring part 2)))
      (if num
	  (let ((results '()))
	    (for-each (lambda (n)
			(set! results (cons dir results)))
		      (iota num))
	    results)
	  (list dir)))))

; Function for parsing a speedwalk string into individual direction commands.
(define parse-speedwalk
  (lambda (input)
    (apply append
	   (map parse-speedwalk-part (all-matches speedwalk-part-regex input)))))

; ===== SPECIAL COMMANDS =====
(define saved-path '())

; Function for reversing a direction.
(define reverse-dir
  (lambda (dir)
    (cond ((string=? dir "n") "s")
	  ((string=? dir "s") "n")
	  ((string=? dir "e") "w")
	  ((string=? dir "w") "e")
	  (else #f))))

; Function for handling special commands.
(define parse-command
  (lambda (cmd)
    (cond ((string=? cmd "reload") (reload-config) '()) ; Reload the config file.
	  ((string=? cmd "path") (set! saved-path '()) '()) ; Start a new path.
	  ((string=? cmd "backtrack") ; Backtrack to the path start.
	   (let ((backpath (reverse (map reverse-dir saved-path))))
	     (set! saved-path '())
	     backpath))
	  (else '()))))

; ===== ALIASES =====
(define aliases (make-hash-table))

; Function for defining an alias.
(define define-alias
  (lambda (alias command)
    (hash-set! aliases alias command)))

; ===== COMMAND SENDING ======
; Function for parsing the input. Returns a list of commands to send
; to the MUD server.
(define parse-input
  (lambda (input)
    (cond ((string=? input "") (list input)) ; Empty input.
	  ((string-contains input ";") (apply append
					      (map parse-input
						   (string-split input #\;)))) ; Multiple commands.
	  ((hash-ref aliases input) (parse-input (hash-ref aliases input))) ; Aliases (recursive).
	  ((string-prefix? "#" input) ; Command.
	   (parse-command (list->string (cdr (string->list input)))))
      ((string-prefix? "/" input) ; Search.
       (begin
         (search-backwards (list->string (cdr (string->list input))))
         '())) ; Note that an empty list is returned.
	  ((regexp-exec speedwalk-check-regex input) ; Speedwalk.
	   (let ((dirs (parse-speedwalk input)))
	     (set! saved-path (apply append (list saved-path dirs)))
	     dirs))
	  (else (list input))))) ; Everything else.

; Add the hook.
(add-hook! send-command-hook
           (lambda (input)
             (map send-command (parse-input input))))

; ===== MUD-SPECIFIC STUFF =====
(define-alias "test" "4n4e")
