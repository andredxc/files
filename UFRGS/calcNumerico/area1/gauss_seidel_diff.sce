function [x,deltax] = gauss_seidel_diff(A,b,x,tol,N)
    
    n = size(A,1)
    xnew = x
    convergiu = %F
    
    k = 1
    while k <= N & ~convergiu
        
        xnew(1) = (b(1) - A(1,2:n)*x(2:n))/A(1,1)
        for i = 2:n-1
            xnew(i) = (b(i) - A(i,1:i-1)*xnew(1:i-1) ...
                            - A(i,i+1:n)*x(i+1:n))/A(i,i)
        end
        xnew(n) = (b(n) - A(n,1:n-1)*xnew(1:n-1))/A(n,n)
        
        deltax = max(abs(x - xnew))
        if deltax < tol then
            convergiu = %T
        end
        if norm(x-xnew,%inf) < 10^(-7)
            disp(k)
            return
        end
        x = xnew
        //disp([k, x'])     
        k = k + 1
    end
    
    if ~convergiu then
        //error('Nao convergiu')
    end
endfunction
