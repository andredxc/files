function encoded = encode_ham(bits, k, n)

% bits -> array containing binary word
% k -> size of the array
% n -> k + p (word length + parity bits)
% y -> encoded word

% Parameter check
p = n - k;
if  pow2(p) > n
    fprintf('p exceds the limit (2^p <= n)');
    return;
end
encoded = zeros([1, n]);

% Copia os bits de entrada para o vetor encoded de saida
j = 1;
for i = 1:length(encoded)

    if i <= pow2(p) && mod(log2(i), 1) == 0 
        % Faz nada, gambiarra, vou mudar depois 
    else
        % Copia da entrada para a saida
        encoded(i) = bits(j);
        j = j + 1;
    end

end

% AJUSTAR PARA P=0
% Computa a paridade
for i = 0:(p-1)
   
    sum = 0;
    for j = pow2(i):2*pow2(i):length(encoded)
        
        fprintf('i = %d, j = %d\n', i, j);
        fprintf('Exp = %d\n', j:pow2(i):(j+pow2(i)-1)); 
        %sum = sum + sum(encoded(j:pow2(i):(j+pow2(i)-1)));
    end
    %fprintf('Sum for p = %d, %d\n', (p-1), sum);
end


